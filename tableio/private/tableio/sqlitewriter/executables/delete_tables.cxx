/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Executable to delete tables within an SQL database
 *
 * @file delete_tables.cxx
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
    "Delete tables from an SQLite database (.db). \n"
    "Commandline arguments:");

    desc.add_options()
        ("help,h", "Show this help message and exit.")
        ("db_in,i", po::value<std::string>()->required(), "The path to the database.")
        ("tables", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "{}"), "The names of the tables to be deleted.");
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



int main(int argc, char* argv[]) {

    // parse command line arguments
    po::variables_map vm = parse_command_line_arguments(argc, argv);

    boost::filesystem::path path = vm["db_in"].as<std::string>();
    std::vector<std::string> tables = vm["tables"].as<std::vector<std::string>>();

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
        std::cout << "Which ones do you want to delete? (Please enter their names separated by spaces.)" << std::endl;
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
        std::cerr << "Error: 0 tables to be deleted were specified, so there's nothing to be done here." << std::endl;
        return 1;
    }
    // check that all specified tables are unique
    if (!all_elems_unique(tables)) {
        std::cerr << "Error: You cannot delete a table multiple times." << std::endl;
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
        std::cerr << "Error: Some of the specified tables don't exist in " << path << "." << std::endl;
        std::cerr << "Available are:" << std::endl;
        for (std::string table : existing_tables) {
            std::cerr << table << " ";
        }
        std::cerr << std::endl;
        return 1;
    }

    // delete the tables
    for (std::string table : tables) {
        delete_table(database, table);
    }

    // close the database
    CHECK_SQLITE3(sqlite3_close(database), database);

    std::cout << "Deletion finished successfully." << std::endl;
    return 0;
}


