/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Executable to merge tables within an SQL database
 *
 * @file merge_tables.cxx
 * @date 2025-06-03
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
    "Merge the fields from multiple parquet files (.parquet) into one file. \n"
    "Commandline arguments:");

    desc.add_options()
        ("help,h", "Show this help message and exit.")
        ("out_path,o", po::value<std::string>()->required(), "The path of the output parquet file.")
        ("parquet_paths,i", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "{}"), "The paths of the parquet files to be merged.")
        ("compression,c", po::value<std::string>()->default_value("uncompressed"), "The compression type to use for the out file.");;
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
        // check that out path doesn't exist yet
        if (boost::filesystem::exists(vm["out_path"].as<std::string>())) {
            std::cerr << "Error: The specified out file: \"" + vm["out_path"].as<std::string>() + "\" already exists." << std::endl;
            exit(1);
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

    boost::filesystem::path out_path = vm["out_path"].as<std::string>();
    std::vector<boost::filesystem::path> parquet_paths = convert_vec_type<boost::filesystem::path>(vm["parquet_paths"].as<std::vector<std::string>>());
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
    // check that all specified parquet paths are unique
    if (!all_elems_unique(parquet_paths)) {
        std::cerr << "Error: You cannot merge a parquet file multiple times with itself." << std::endl;
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

    // variables to open and read parquet files
    std::shared_ptr<arrow::io::ReadableFile> file;
    std::shared_ptr<parquet::arrow::FileReader> reader;
    std::shared_ptr<arrow::Schema> schema;

    // get field names and datatypes of each parquet file
    // and get their number of row groups
    // also check for a field "event_no" in them
    std::map<boost::filesystem::path, std::vector<std::string>> field_names_map;
    std::map<boost::filesystem::path, std::vector<std::shared_ptr<arrow::DataType>>> field_types_map;
    std::pair<std::vector<std::string>, std::vector<std::shared_ptr<arrow::DataType>>> fields_tmp;
    size_t event_no_idx_tmp;
    std::map<boost::filesystem::path, size_t> event_no_idcs_map;
    std::vector<size_t> nums_row_groups;

    for (boost::filesystem::path parquet_path: parquet_paths) {
        PARQUET_ASSIGN_OR_THROW(file, arrow::io::ReadableFile::Open(parquet_path.string()));
        PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(file, arrow::default_memory_pool()));
        PARQUET_THROW_NOT_OK(reader->GetSchema(&schema));

        event_no_idx_tmp = get_index<std::string>("event_no", schema->field_names());
        if (event_no_idx_tmp == schema->field_names().size()) {
            std::cerr << "Error: The parquet file " << parquet_path << " has no field \"event_no\", so it was not created with parquetwriter." << std::endl; 
            return 1;
        }
        else {
            event_no_idcs_map[parquet_path] = event_no_idx_tmp;
        }

        fields_tmp = get_fields(schema);
        field_names_map[parquet_path] = fields_tmp.first;
        field_types_map[parquet_path] = fields_tmp.second;

        nums_row_groups.push_back(reader->num_row_groups());

        schema.reset();
        reader.reset();
        PARQUET_THROW_NOT_OK(file->Close());
    }
    // check that every field only appears once in all files combined
    bool found_duplicate_fields = false;
    for (std::pair<std::string, size_t> key_count : count_occurence(field_names_map)) {
        if (key_count.second > 1) {
            std::cerr << "The field \"" << key_count.first << "\" appears " << key_count.second << " times in the specified parquet files." << std::endl;
            found_duplicate_fields = true;
        }
    }
    if (found_duplicate_fields) {
        std::cerr << "Error: Found duplicate fields in:" << std::endl;
        for (boost::filesystem::path parquet_path : parquet_paths) {
            std::cerr << parquet_path.string() << " ";
        }
        std::cerr << std::endl;
        return 1;
    }

    if (!all_elems_equal(nums_row_groups)) {
        std::cerr << "Error: Some parquet files have different lengths (number of row groups):" << std::endl;
        for (size_t j = 0; j < parquet_paths.size(); j++) {
            std::cerr << parquet_paths[j].string() << "(" << nums_row_groups[j] << ") ";
        }
        std::cerr << std::endl;
        return 1;
    }

    size_t num_row_groups = nums_row_groups[0];

    // get flattened column names and types    
    std::vector<std::string> all_field_names;
    std::vector<std::shared_ptr<arrow::DataType>> all_field_types;
    for (boost::filesystem::path parquet_path : parquet_paths) {
        all_field_names.insert(all_field_names.end(), field_names_map[parquet_path].begin(), field_names_map[parquet_path].end());
        all_field_types.insert(all_field_types.end(), field_types_map[parquet_path].begin(), field_types_map[parquet_path].end());
    }
    // get merged schema
    std::shared_ptr<arrow::Schema> merged_schema = get_schema(all_field_names, all_field_types);

    // open out file and writer
    std::shared_ptr<arrow::io::FileOutputStream> out_file;
    PARQUET_ASSIGN_OR_THROW(out_file, arrow::io::FileOutputStream::Open(out_path.string()));
    parquet::WriterProperties::Builder writer_props_builder;
    writer_props_builder.compression(compression);
    parquet::ArrowWriterProperties::Builder arrow_writer_props_builder;
    std::shared_ptr<parquet::arrow::FileWriter> out_writer;
    PARQUET_ASSIGN_OR_THROW(out_writer, parquet::arrow::FileWriter::Open(*merged_schema, arrow::default_memory_pool(), out_file, writer_props_builder.build(), arrow_writer_props_builder.build()));

    // loop over all row groups
    std::vector<std::shared_ptr<arrow::ChunkedArray>> columns;
    std::shared_ptr<arrow::Table> table;
    std::vector<std::shared_ptr<arrow::ChunkedArray>> merged_columns;
    std::vector<std::shared_ptr<arrow::ChunkedArray>> event_no_columns;
    std::shared_ptr<arrow::Table> merged_table;
    for (size_t n = 0; n < num_row_groups; n++) {

        // loop over all files and merge their respective row groups
        for (boost::filesystem::path parquet_path : parquet_paths) {
            PARQUET_ASSIGN_OR_THROW(file, arrow::io::ReadableFile::Open(parquet_path.string()));
            PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(file, arrow::default_memory_pool()));

            // read the row group
            PARQUET_THROW_NOT_OK(reader->RowGroup(n)->ReadTable(&table));
            columns = table->columns();

            // save the event_no column
            event_no_columns.push_back(columns[event_no_idcs_map[parquet_path]]);

            // delete the event_no column and insert into merged columns
            columns.erase(columns.begin() + event_no_idcs_map[parquet_path]);
            merged_columns.insert(merged_columns.end(), columns.begin(), columns.end());
        }

        // check that the row groups all have the same number of rows
        if (!all_chunked_arrays_equal_length(event_no_columns)) {
            std::cerr << "Error: At row group " << n << " the specified files have different lengths, so they cannot be merged." << std::endl;
            return 1;
        }
        // check that all event_no columns are equal
        if (!all_chunked_arrays_equal(event_no_columns)) {
            std::cerr << "Error: At row group " << n << " the specified files' event_nos don't match up, so they cannot be merged." << std::endl;
            return 1;
        }
        // add the event_no column to the merged columns
        merged_columns.push_back(event_no_columns[0]);

        // write the merged table to the out file
        merged_table = arrow::Table::Make(merged_schema, merged_columns);
        PARQUET_THROW_NOT_OK(out_writer->WriteTable(*merged_table, merged_table->num_rows()));

        reader.reset();
        PARQUET_THROW_NOT_OK(file->Close());
    }

    merged_schema.reset();
    PARQUET_THROW_NOT_OK(out_writer->Close());
    PARQUET_THROW_NOT_OK(out_file->Close());

    std::cout << "Merging finished successfully." << std::endl;
    return 0;
}


