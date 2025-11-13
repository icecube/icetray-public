/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Collection of utils functions for SQLite Database operations
 *
 * @file sqlite_utils.h
 * @date 2025-06-03
 * @author lepeler
 *
*/

#ifndef SQLITE_UTILS_H_INCLUDED
#define SQLITE_UTILS_H_INCLUDED


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

#include <sqlite3.h>
#include <boost/filesystem.hpp>



template<typename T>
inline
bool elem_in_vec(T elem, const std::vector<T> &vec) {
    // is elem in vec?

    return (std::find(vec.begin(), vec.end(), elem) != vec.end());
}


template<typename T, typename U>
inline
std::vector<T> convert_vec_type(const std::vector<U> &vec) {
    // convert vector<U> to vector<T>
    // U needs to be implicitly convertable to T

    std::vector<T> ret_vec;
    for (U elem : vec) {
        ret_vec.push_back(elem);
    }
    return ret_vec;
}


template<typename T, typename U>
inline
std::map<U, size_t> count_occurence(const std::map<T, std::vector<U>> &vec_map) {
    // return a map of elements to their occurence in the vector map

    std::map<T, size_t> elem_counts;
    for (std::pair<T, std::vector<U>> vec : vec_map) {
        for (U elem : vec.second) {
            elem_counts[elem]++;
        }
    }
    return elem_counts;
}


template<typename T>
inline
bool all_elems_unique(const std::vector<T> &vec) {
    // are all elements of the vector unique (i.e. are non of them duplicate)?

    std::vector<T> already_checked;
    for (T elem : vec) {
        if (elem_in_vec(elem, already_checked))
            {return false;}
        already_checked.push_back(elem);
    }
    return true;
}


template<typename T>
inline
bool all_elems_equal(const std::vector<T> &vec) {
    // are all elements of the vector equal?

    if (vec.empty())
        {return true;}

    for (T elem : vec) {
        if (elem != vec.back())
            {return false;}
    }
    return true;
}


template<typename T>
inline
size_t get_index(const std::vector<T> &vec, T elem) {
    // get the index of elem in vec,
    // if it isn't present return vec.size()

    return std::distance(vec.begin(), std::find(vec.begin(), vec.end(), elem));
}


inline
const int64_t charptr_to_int(const char* charptr) {
    // convert a char* to long int
    // return -1, if the charptr is a nullptr

    if (charptr)
        {return std::atoi(charptr);}
    else
        {return -1;}
}


inline
void CHECK_SQLITE3(int rc, sqlite3* &database) {
    // check the return code of an sqlite3 operation

    if (!(rc == SQLITE_OK || rc == SQLITE_DONE)) {
        throw std::runtime_error("SQLite: " + std::string(sqlite3_errmsg(database)));
    }
}


inline
bool make_step(sqlite3_stmt* &sql_stmt) {
    // make a step on the sql statement (i.e. move to the next row)
    // and return whether the statement is still at a valid row or not (i.e. all rows have already been queried)

    return (sqlite3_step(sql_stmt) == SQLITE_ROW);
}


inline
const char* sqlite3_read(sqlite3_stmt* &sql_stmt, size_t i) {
    // call the sqlite3_column_text() function
    // and cast the output to const char*

    return reinterpret_cast<const char*>(sqlite3_column_text(sql_stmt, i));
}


inline
std::vector<std::string> get_tables(sqlite3* &database) {
    // get a vector of tables in the database

    std::string sql_command = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';";
    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    std::vector<std::string> tables;
    while (make_step(sql_stmt)) {
        tables.push_back(sqlite3_read(sql_stmt, 0));
    }
    CHECK_SQLITE3(sqlite3_finalize(sql_stmt), database);

    return tables;
}


inline
bool table_has_event_no(sqlite3* &database, std::string table_name) {
    // does the table have a column named event_no of type INTEGER?

    std::string sql_command = "PRAGMA table_info(\"" + table_name + "\");";
    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    while (make_step(sql_stmt)) {
        if (std::string(sqlite3_read(sql_stmt, 1)) == "event_no" && std::string(sqlite3_read(sql_stmt, 2)) == "INTEGER") {
            CHECK_SQLITE3(sqlite3_finalize(sql_stmt), database);
            return true;
        }
    }
    CHECK_SQLITE3(sqlite3_finalize(sql_stmt), database);
    return false;
}


inline
std::pair<std::vector<std::string>, std::vector<std::string>> get_columns(sqlite3* &database, std::string table_name) {
    // get a the column names and datatypes in the table (without event_no)

    std::string sql_command = "PRAGMA table_info(\"" + table_name + "\");";
    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::string column_name;
    while (make_step(sql_stmt)) {
        column_name = sqlite3_read(sql_stmt, 1);
        if (column_name != "event_no") {
            column_names.push_back(column_name);
            column_types.push_back(sqlite3_read(sql_stmt, 2));
        }
    }
    CHECK_SQLITE3(sqlite3_finalize(sql_stmt), database);

    return std::make_pair(column_names, column_types);
}


inline
void create_table(sqlite3* &database, std::string table_name, const std::vector<std::string> &column_names, const std::vector<std::string> &column_types) {
    // create a new table in the database (add event_no to specified columns)

    std::string sql_command = "CREATE TABLE \"" + table_name + "\"(";
    for (size_t i = 0; i < column_names.size(); i++) {
        sql_command += "\"" + column_names[i] + "\" " + column_types[i] + ", ";
    }
    sql_command += "\"event_no\" INTEGER NOT NULL); CREATE INDEX \"event_no_" + table_name + "\" ON \"" + table_name +"\" (\"event_no\");";

    CHECK_SQLITE3(sqlite3_exec(database, sql_command.c_str(), 0, 0, nullptr), database);
}


inline
sqlite3_stmt* get_insertion_statement(sqlite3* &database, std::string table_name) {
    // prepare insertion statement

    std::vector<std::string> column_names = get_columns(database, table_name).first;

    std::string sql_command =  "INSERT INTO \"" + table_name + "\"(";
    for (std::string column_name : column_names) {
        sql_command += "\"" + column_name + "\", ";
    }
    sql_command += "\"event_no\") VALUES (";
    for (size_t i = 0; i < column_names.size(); i++) {
        sql_command += "?, ";
    }
    sql_command += "?);";

    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    return sql_stmt;
}


inline
sqlite3_stmt* get_query_statement(sqlite3* &database, std::string table_name) {
    // prepare query statement

    std::string sql_command = "SELECT ";
    for (std::string column_name : get_columns(database, table_name).first) {
        sql_command += "\"" + column_name + "\", ";
    }
    sql_command += "\"event_no\" FROM \"" + table_name + "\";";

    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    return sql_stmt;
}


inline
void delete_table(sqlite3* &database, std::string table_name) {
    // delete a table from the database

    std::string sql_command = "DROP TABLE \"" + table_name + "\";";

    CHECK_SQLITE3(sqlite3_exec(database, sql_command.c_str(), 0, 0, nullptr), database);
}


inline
const size_t get_event_no_offset(sqlite3* &database, std::string table_name) {
    // get the maximum event_no in the database + 1
    // if there is no column called event_no in the database return 0

    std::string sql_command = "SELECT MAX(event_no) FROM \"" + table_name + "\";";
    sqlite3_stmt* sql_stmt;
    CHECK_SQLITE3(sqlite3_prepare_v2(database, sql_command.c_str(), -1, &sql_stmt, nullptr), database);

    size_t event_no_offset = 0;
    if (make_step(sql_stmt)) {
        event_no_offset = charptr_to_int(sqlite3_read(sql_stmt, 0)) + 1;
    }
    CHECK_SQLITE3(sqlite3_finalize(sql_stmt), database);

    return event_no_offset;
}



#endif //SQLITE_UTILS_H_INCLUDED

