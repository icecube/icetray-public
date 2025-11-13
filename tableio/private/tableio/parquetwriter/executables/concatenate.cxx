/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Executable to concatenate parquet files
 *
 * @file merge_databases.cxx
 * @date 2025-07-27
 * @author lepeler
 *
*/

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <unordered_set>

#include <arrow/api.h>
#include <arrow/io/file.h>
#include <arrow/compute/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/arrow/reader.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <tableio/parquetwriter/parquet_utils.h>

namespace po = boost::program_options;



po::variables_map parse_command_line_arguments(int argc, char* argv[]) {
    // define available options
    po::options_description desc (
    "Merge multiple Parquet files (.parquet) into one. If the out file already exists it will be appended to. (The files all need to have the same schemas for this to work.) \n"
    "Commandline arguments:");

    desc.add_options()
        ("help,h", "Show this help message and exit.")
        ("parquet_out,o", po::value<std::string>()->required(), "The path under which to save the merged file.")
        ("parquet_files,i", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "{}"), "The paths of the files to be merged.")
        ("compression,c", po::value<std::string>()->default_value("uncompressed"), "The compression type to use for the out file.");
    // parse options
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        // print help message if asked for
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        }
        // check for all required arguments
        po::notify(vm);
        // check whether database exists
        if (boost::filesystem::exists(vm["parquet_out"].as<std::string>())) {
            std::cout << "The specified out path: \"" + vm["parquet_out"].as<std::string>() + "\" already exists, appending..." << std::endl;
        }
        else {
            std::cout << "The specified out path: \"" + vm["parquet_out"].as<std::string>() + "\" does not exist yet, creating new file..." << std::endl;
        }
    }
    catch (const po::error &ex) {
        std::cerr << ex.what() << std::endl;
        std::cout << desc << std::endl;
        exit(1);
    }

    return vm;
}



int main(int argc, char* argv[]) {

    // parse command line arguments
    po::variables_map vm = parse_command_line_arguments(argc, argv);

    boost::filesystem::path out_path = vm["parquet_out"].as<std::string>();
    std::vector<boost::filesystem::path> parquet_paths = convert_vec_type<boost::filesystem::path>(vm["parquet_files"].as<std::vector<std::string>>());
    parquet::Compression::type compression = get_compression_type(vm["compression"].as<std::string>());

    // if parquet_paths is empty prompt for them
    if (parquet_paths.empty()) {
        std::cout << "Which parquet files do you want to merge into " << out_path << "? (Please enter their paths separated by spaces.)" << std::endl;
        std::cout << "-> ";
        std::string parquet_paths_input;
        std::getline(std::cin, parquet_paths_input);
        std::istringstream iss(parquet_paths_input);
        while (iss >> parquet_paths_input) {
            parquet_paths.push_back(parquet_paths_input);
        }
    }
    // if parquet_paths is still empty, abort
    if (parquet_paths.empty()) {
        std::cerr << "Error: 0 parquet files to be merged were specified, so there's nothing to be done here." << std::endl;
        return 1;
    }
    // check that all specified parquet paths exist
    bool all_parquets_exist = true;
    for (boost::filesystem::path parquet_path : parquet_paths) {
        if (!boost::filesystem::exists(parquet_path)) {
            std::cerr << "The parquet path " << parquet_path << " does not exist." << std::endl;
            all_parquets_exist = false;
        }
    }
    if (!all_parquets_exist) {
        std::cerr << "Error: Some of the specified parquet paths don't exist." << std::endl;
        return 1;
    }

    // if out file doesn't exist yet, 'seed' it with the first parquet file
    if (!boost::filesystem::exists(out_path)) {
        boost::filesystem::copy_file(parquet_paths.front(), out_path);
        parquet_paths.erase(parquet_paths.begin());
    }

    // get out file schema
    std::shared_ptr<arrow::io::ReadableFile> out_file_read;
    std::shared_ptr<parquet::arrow::FileReader> out_reader;
    std::shared_ptr<arrow::Schema> out_schema;
    PARQUET_ASSIGN_OR_THROW(out_file_read, arrow::io::ReadableFile::Open(out_path.string()));
    PARQUET_ASSIGN_OR_THROW(out_reader, parquet::arrow::OpenFile(out_file_read, arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(out_reader->GetSchema(&out_schema));

    // open out file and writer in append mode (had to be implemented very inefficiently b.c. the arrow api doesn't support it natively)
    std::shared_ptr<arrow::io::FileOutputStream> out_file;
    std::shared_ptr<parquet::arrow::FileWriter> out_writer;
    std::pair<std::shared_ptr<arrow::io::FileOutputStream>, std::shared_ptr<parquet::arrow::FileWriter>> append_tmp;

    append_tmp = Open_as_Append(out_path.string(), compression);
    out_file = append_tmp.first;
    out_writer = append_tmp.second;

    // get the column index of "event_no" and check that it is valid
    int event_no_index = out_schema->GetFieldIndex("event_no");
    if (event_no_index == -1) {
        std::cerr << "Error: The out file " << out_path << " has no field \"event_no\", so it was not created with parquetwriter." << std::endl; 
        return 1;
    }

    // loop over all parquet files and insert them into the merged file
    std::shared_ptr<arrow::io::ReadableFile> file;
    std::shared_ptr<parquet::arrow::FileReader> reader;
    std::shared_ptr<arrow::Schema> schema;
    size_t num_row_groups;
    uint64_t event_no_offset;
    std::shared_ptr<arrow::Table> table;

    for (boost::filesystem::path parquet_path : parquet_paths) {
        PARQUET_ASSIGN_OR_THROW(file, arrow::io::ReadableFile::Open(parquet_path.string()));
        PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(file, arrow::default_memory_pool()));
        PARQUET_THROW_NOT_OK(reader->GetSchema(&schema));
        num_row_groups = reader->num_row_groups();

        // check that the parquet file has the same schema as the out file
        // also check for a field "event_no"
        if (!elem_in_vec<std::string>("event_no", schema->field_names())) {
            std::cerr << "Error: " << parquet_path << " has no field \"event_no\", so it was not created with parquetwriter." << std::endl; 
            return 1;
        }
        if (!schema->Equals(*out_schema)) {
            std::cerr << "Error: " << parquet_path << " has a different schema than the out file " << out_path << "." << std::endl;
            return 1;
        }

        // get event_no_offset
        event_no_offset = get_event_no_offset(out_reader, event_no_index);

        // iterate over all row groups and insert them into the out file
        for (size_t n = 0; n < num_row_groups; n++) {
            // read the row group
            PARQUET_THROW_NOT_OK(reader->RowGroup(n)->ReadTable(&table));
            // offset event_no by event_no_offset
            PARQUET_THROW_NOT_OK(offset_event_no_column(table, event_no_index, event_no_offset));
            // and write the table to the out file
            PARQUET_THROW_NOT_OK(out_writer->WriteTable(*table, table->num_rows()));
        }
        reader.reset();
        PARQUET_THROW_NOT_OK(file->Close());
    }

    out_reader.reset();
    PARQUET_THROW_NOT_OK(out_file_read->Close());
    PARQUET_THROW_NOT_OK(out_writer->Close());
    PARQUET_THROW_NOT_OK(out_file->Close());

    std::cout << "Merging finished successfully." << std::endl;
    return 0;
}


