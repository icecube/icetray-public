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

#include <sqlite3.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <tableio/sqlitewriter/sqlite_utils.h>

namespace po = boost::program_options;



po::variables_map parse_command_line_arguments(int argc, char* argv[]) {
    // define available options
    po::options_description desc (
    "Merge multiple tables in one SQLite database (.db) into one and add it to the database. \n"
    "Commandline arguments:");

    desc.add_options()
        ("help,h", "Show this help message and exit.")
        ("db_in,i", po::value<std::string>()->required(), "The path to the database.")
        ("tables", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "{}"), "The names of the tables to be merged.")
        ("name", po::value<std::string>()->default_value(""), "The name to give to the merged table.");
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
        if (!boost::filesystem::exists(vm["db_in"].as<std::string>())) {
            std::cerr << "Error: The specified database: \"" + vm["db_in"].as<std::string>() + "\" doesn't exist." << std::endl;
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


bool make_and_validate_step(std::map<std::string, sqlite3_stmt*> query_stmts) {
    // call make_step() on all query statements
    // and check, whether all returned the same state

    if (query_stmts.empty())
        {return false;}

    std::vector<bool> not_finisheds;
    for (std::pair<std::string, sqlite3_stmt*> query_stmt : query_stmts) {
        not_finisheds.push_back(make_step(query_stmt.second));

        if (not_finisheds.back() != not_finisheds[0]) {
            std::cerr << "Error: Detected different lengths of the tables being merged." << std::endl;
            return false;
        }
    }

    return not_finisheds[0];
}



int main(int argc, char* argv[]) {

    // parse command line arguments
    po::variables_map vm = parse_command_line_arguments(argc, argv);

    boost::filesystem::path path = vm["db_in"].as<std::string>();
    std::vector<std::string> tables = vm["tables"].as<std::vector<std::string>>();
    std::string merged_name = vm["name"].as<std::string>();

    // open database
    sqlite3 *database;
    CHECK_SQLITE3(sqlite3_open(path.c_str(), &database), database);

    std::vector<std::string> existing_tables = get_tables(database);

    // if tables is empty prompt for them
    if (tables.empty()) {
        std::cout << "The following tables exist in the database:" << std::endl;
        for (std::string table : existing_tables) {
            std::cout << table << " ";
        }
        std::cout << std::endl;
        std::cout << "Which ones do you want to merge? (Please enter their names separated by spaces.)" << std::endl;
        std::cout << "-> ";
        std::string tables_input;
        std::getline(std::cin, tables_input);
        std::istringstream iss(tables_input);
        while (iss >> tables_input) {
            tables.push_back(tables_input);
        }
    }
    // if tables is still empty, abort
    if (tables.empty()) {
        std::cerr << "Error: 0 tables to be merged were specified, so there's nothing to be done here." << std::endl;
        return 1;
    }
    // check that all specified tables are unique
    if (!all_elems_unique(tables)) {
        std::cerr << "Error: You cannot merge a table multiple times with itself." << std::endl;
        return 1;
    }
    // check that all specified tables exist in the database
    bool all_tables_exist = true;
    for (std::string table : tables) {
        if (!elem_in_vec(table, existing_tables)) {
            std::cerr << "The database does not have a table named \"" << table << "\"." << std::endl;
            all_tables_exist = false;
        }
    }
    if (!all_tables_exist) {
        std::cerr << "Error: Some tables don't exist in " << path << "." << std::endl;
        std::cerr << "Available are:" << std::endl;
        for (std::string table : existing_tables) {
            std::cerr << table << " ";
        }
        std::cerr << std::endl;
        return 1;
    }

    // get column names and datatypes of each table
    // also check for event_no in them
    std::map<std::string, std::vector<std::string>> column_names_map;
    std::map<std::string, std::vector<std::string>> column_types_map;
    std::pair<std::vector<std::string>, std::vector<std::string>> columns_tmp;
    std::map<std::string, sqlite3_stmt*> query_stmts;
    for (std::string table : tables) {
        if (!table_has_event_no(database, table)) {
            std::cerr << "Error: The table \"" << table << "\" has no column \"event_no\", so it was not created with sqlitewriter." << std::endl; 
            return 1;
        }
        columns_tmp = get_columns(database, table);
        column_names_map[table] = columns_tmp.first;
        column_types_map[table] = columns_tmp.second;
        query_stmts[table] = get_query_statement(database, table);
    }
    // check that every column only appears once in all tables combined
    bool found_duplicate_columns = false;
    for (std::pair<std::string, size_t> key_count : count_occurence(column_names_map)) {
        if (key_count.second > 1) {
            std::cerr << "The column \"" << key_count.first << "\" appears " << key_count.second << " times in the specified tables." << std::endl;
            found_duplicate_columns = true;
        }
    }
    if (found_duplicate_columns) {
        std::cerr << "Error: Found duplicate columns in:" << std::endl;
        for (std::string table : tables) {
            std::cerr << table << " ";
        }
        std::cerr << std::endl;
        return 1;
    }

    // if merged_name is empty prompt for it
    if (merged_name.empty()) {
        std::cout << "What should the merged table be called?" << std::endl;
        std::cout << "-> ";
        std::cin >> merged_name;
    }
    // if merged_name is still empty, abort
    if (merged_name.empty()) {
        std::cerr << "Error: The merged table cannot have no name." << std::endl;
        return 1;
    }
    // check, that merged_name is not already a column in the database
    if (elem_in_vec(merged_name, existing_tables)) {
        std::cerr << "Error: The merged table cannot have the name of an already existing table in the database." << std::endl;
        return 1;
    }

    // get flattened column names and types
    std::vector<std::string> all_column_names;
    std::vector<std::string> all_column_types;
    for (std::string table : tables) {
        all_column_names.insert(all_column_names.end(), column_names_map[table].begin(), column_names_map[table].end());
        all_column_types.insert(all_column_types.end(), column_types_map[table].begin(), column_types_map[table].end());
    }
    // create merged table
    create_table(database, merged_name, all_column_names, all_column_types);
    sqlite3_stmt* insertion_stmt = get_insertion_statement(database, merged_name);
    size_t column_count = sqlite3_bind_parameter_count(insertion_stmt);

    // loop over all rows in all tables and insert them into the new merged table
    CHECK_SQLITE3(sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, nullptr), database);

    size_t row_no = 0;
    std::vector<size_t> event_nos;
    while (make_and_validate_step(query_stmts)) {
        row_no++;
        for (std::string table : tables) {
            for (size_t i = 0; i < column_names_map[table].size(); i++) {
                // bind value directly into the insertion statement
                CHECK_SQLITE3(sqlite3_bind_text(insertion_stmt, get_index(all_column_names, column_names_map[table][i])+1, sqlite3_read(query_stmts[table], i), -1, SQLITE_TRANSIENT), database);
            }
            event_nos.push_back(std::atoi(sqlite3_read(query_stmts[table], column_names_map[table].size())));
        }
        if (!all_elems_equal(event_nos)) {
            std::cerr << "Error: At row " << row_no << " the specified tables' event_nos don't match up, so they cannot be merged." << std::endl;
            return 1;
        }
        CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt, column_count, event_nos.back()), database);
        CHECK_SQLITE3(sqlite3_step(insertion_stmt), database);
        CHECK_SQLITE3(sqlite3_reset(insertion_stmt), database);
        event_nos.clear();
    }

    CHECK_SQLITE3(sqlite3_exec(database, "COMMIT;", 0, 0, nullptr), database);

    for (std::string table : tables) {
        CHECK_SQLITE3(sqlite3_finalize(query_stmts[table]), database);
    }
    CHECK_SQLITE3(sqlite3_finalize(insertion_stmt), database);
    CHECK_SQLITE3(sqlite3_close(database), database);

    std::cout << "Merging finished successfully." << std::endl;
    return 0;
}


