/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Executable to concatenate SQL databases
 *
 * @file concatenate.cxx
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

#include <sqlite3.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <tableio/sqlitewriter/sqlite_utils.h>

namespace po = boost::program_options;



po::variables_map parse_command_line_arguments(int argc, char* argv[]) {
    // define available options
    po::options_description desc (
    "Merge multiple SQLite database (.db) into one. If the out file already exists it will be appended to. (The databases all need to have the same schemas for this to work.) \n"
    "Commandline arguments:");

    desc.add_options()
        ("help,h", "Show this help message and exit.")
        ("db_out,o", po::value<std::string>()->required(), "The path under which to save the merged database.")
        ("databases,i", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "{}"), "The paths of the databases to be merged.");
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
        if (boost::filesystem::exists(vm["db_out"].as<std::string>())) {
            std::cout << "The specified out path: \"" + vm["db_out"].as<std::string>() + "\" already exists, appending..." << std::endl;
        }
        else {
            std::cout << "The specified out path: \"" + vm["db_out"].as<std::string>() + "\" does not exist yet, creating new file..." << std::endl;
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

    boost::filesystem::path out_path = vm["db_out"].as<std::string>();
    std::vector<boost::filesystem::path> database_paths = convert_vec_type<boost::filesystem::path>(vm["databases"].as<std::vector<std::string>>());

    // if databases is empty prompt for them
    if (database_paths.empty()) {
        std::cout << "Which databases do you want to merge into " << out_path << "? (Please enter their paths separated by spaces.)" << std::endl;
        std::cout << "-> ";
        std::string databases_input;
        std::getline(std::cin, databases_input);
        std::istringstream iss(databases_input);
        while (iss >> databases_input) {
            database_paths.push_back(databases_input);
        }
    }
    // if databases is still empty, abort
    if (database_paths.empty()) {
        std::cerr << "Error: 0 databases to be merged were specified, so there's nothing to be done here." << std::endl;
        return 1;
    }
    // check that all specified database paths exist
    bool all_databases_exist = true;
    for (boost::filesystem::path database_path : database_paths) {
        if (!boost::filesystem::exists(database_path)) {
            std::cerr << "The database path " << database_path << " does not exist." << std::endl;
            all_databases_exist = false;
        }
    }
    if (!all_databases_exist) {
        std::cerr << "Error: Some of the specified database paths don't exist." << std::endl;
        return 1;
    }

    // if out database doesn't exist yet, 'seed' it with the first database
    if (!boost::filesystem::exists(out_path)) {
        boost::filesystem::copy_file(database_paths.front(), out_path);
        database_paths.erase(database_paths.begin());
    }

    // open out database
    sqlite3 *out_database;
    CHECK_SQLITE3(sqlite3_open(out_path.c_str(), &out_database), out_database);

    // get insertion and query statements to the merged database
    // also check for event_no in each table
    std::vector<std::string> all_tables = get_tables(out_database);
    std::map<std::string, std::vector<std::string>> column_names_map;
    std::map<std::string, std::vector<std::string>> column_types_map;
    std::pair<std::vector<std::string>, std::vector<std::string>> columns_tmp;
    std::map<std::string, sqlite3_stmt*> insertion_stmts;
    std::map<std::string, size_t> param_counts;
    for (std::string table : all_tables) {
        if (!table_has_event_no(out_database, table)) {
            std::cerr << "Error: The table \"" << table << "\" of the out database " << out_path << " has no column \"event_no\", so it was not created with sqlitewriter." << std::endl; 
            return 1;
        }
        columns_tmp = get_columns(out_database, table);
        column_names_map[table] = columns_tmp.first;
        column_types_map[table] = columns_tmp.second;
        insertion_stmts[table] = get_insertion_statement(out_database, table);
        param_counts[table] = sqlite3_bind_parameter_count(insertion_stmts[table]);
    }

    // loop over all databases and insert them into the merged database
    sqlite3* database;
    std::map<std::string, sqlite3_stmt*> query_stmts;
    for (boost::filesystem::path database_path : database_paths) {
        CHECK_SQLITE3(sqlite3_open(database_path.c_str(), &database), database);

        // check that the database has the same query statements as the out database
        // also check for event_no in each table (i.e. their schemas are equal)
        for (std::string table : all_tables) {
            if (!table_has_event_no(database, table)) {
                std::cerr << "Error: The table \"" << table << "\" of " << database_path << " has no column \"event_no\", so it was not created with sqlitewriter." << std::endl; 
                return 1;
            }
            if (get_columns(database, table) != std::make_pair(column_names_map[table], column_types_map[table])) {
                std::cerr << "Error: " << database_path << " has a different schema than the out database " << out_path << "." << std::endl;
                return 1;
            }
            query_stmts[table] = get_query_statement(database, table);
        }

        // loop over all tables and insert them into the merged database
        CHECK_SQLITE3(sqlite3_exec(out_database, "BEGIN TRANSACTION;", 0, 0, nullptr), out_database);

        size_t event_no_offset;
        for (std::string table : all_tables) {
            event_no_offset = get_event_no_offset(out_database, table);
            while (make_step(query_stmts[table])) {
                for (size_t i = 0; i < param_counts[table] - 1; i++) {
                    // bind value directly into the insertion statement
                    CHECK_SQLITE3(sqlite3_bind_text(insertion_stmts[table], i+1, sqlite3_read(query_stmts[table], i), -1, SQLITE_TRANSIENT), out_database);
                }
                // bind event_no separately because it needs to be offset in order to not have multiples
                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmts[table], param_counts[table], std::atoi(sqlite3_read(query_stmts[table], param_counts[table]-1)) + event_no_offset), out_database);
                // insert the row
                CHECK_SQLITE3(sqlite3_step(insertion_stmts[table]), out_database);
                CHECK_SQLITE3(sqlite3_reset(insertion_stmts[table]), out_database);
            }
        }
        CHECK_SQLITE3(sqlite3_exec(out_database, "COMMIT;", 0, 0, nullptr), out_database);

        for (std::string table : all_tables) {
            CHECK_SQLITE3(sqlite3_finalize(query_stmts[table]), database);
        }
        CHECK_SQLITE3(sqlite3_close(database), database);
    }

    for (std::string table : all_tables) {
        CHECK_SQLITE3(sqlite3_finalize(insertion_stmts[table]), out_database);
    }
    CHECK_SQLITE3(sqlite3_close(out_database), out_database);

    std::cout << "Merging finished successfully." << std::endl;
    return 0;
}


