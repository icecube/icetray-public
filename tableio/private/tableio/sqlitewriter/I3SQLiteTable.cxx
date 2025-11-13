/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Implementation of I3SQLiteTable
 *
 * @file I3SQLiteTable.cxx
 * @date 2025-02-16
 * @author lepeler
 *
*/

#include <string>
#include <cstdint>

#include <sqlite3.h>
#include <boost/filesystem.hpp>

#include <tableio/I3Table.h>
#include <tableio/I3TableRow.h>
#include <tableio/I3TableService.h>

#include "I3SQLiteTable.h"
#include "sqlite_utils.h"



I3SQLiteTable::I3SQLiteTable(I3TableService &service,
                             std::string tableName,
                             I3TableRowDescriptionConstPtr description,
                             boost::filesystem::path path)
                             : I3Table(service,
                                       tableName,
                                       description)
{
    db_path_ = path;
    num_fields_ = description_->GetFieldNames().size();
    event_no_ = 0;
    if (tableName == "I3EventHeader")
        {skip_cols_ = 0;}
    else
        {skip_cols_ = 5;}

    CreateTable();
}


I3SQLiteTable::~I3SQLiteTable()
{
    // close the database
    CHECK_SQLITE3(sqlite3_finalize(insertion_stmt_), database_);
    CHECK_SQLITE3(sqlite3_close(database_), database_);
}


void I3SQLiteTable::CreateTable()
{
    // open database
    CHECK_SQLITE3(sqlite3_open(db_path_.c_str(), &database_), database_);

    // get sqlite names and types of columns
    std::vector<std::string> sqlite_names = description_->GetFieldNames();
    std::vector<std::string> sqlite_types;
    for (const I3Datatype &type : description_->GetFieldTypes()) {
        sqlite_types.push_back(GetSQLiteType(type));
    }

    // check that all a correspondong SQLite DataType was found for all I3Datatypes 
    bool all_types_fine = true;
    for (size_t i = 0; i < sqlite_types.size(); i++) {
        if (sqlite_types[i] == "NULL") {
            log_warn("Can't handle type: '%s' (field: '%s').", description_->GetFieldTypes()[i].AsString().c_str(), description_->GetFieldNames()[i].c_str());
            all_types_fine = false;
        }
    }
    if (!all_types_fine) {
        log_fatal("For some I3Datatypes no corresponding SQLite DataType was found.");
    }

    // skip first skip_cols_ columns
    sqlite_names.erase(sqlite_names.begin(), sqlite_names.begin() + skip_cols_);
    sqlite_types.erase(sqlite_types.begin(), sqlite_types.begin() + skip_cols_);

    // create table
    create_table(database_, name_, sqlite_names, sqlite_types);

    // prepare insertion statement
    insertion_stmt_ = get_insertion_statement(database_, name_);
}


void I3SQLiteTable::WriteRows(I3TableRowConstPtr row)
{
    I3TableRowPtr rows = boost::const_pointer_cast<I3TableRow>(row);
    rows->SetEnumsAreInts(true);
    I3Datatype type;
    // insert data
    sqlite3_exec(database_, "BEGIN TRANSACTION;", 0, 0, nullptr);
    for (size_t n = 0; n < rows->GetNumberOfRows(); n++) {
        rows->SetCurrentRow(n);
        for (size_t i = skip_cols_; i < num_fields_; i++) {
            type = description_->GetFieldTypes()[i];
            switch (type.kind) {
                case I3Datatype::TypeClass::Float:
                    switch (type.size) {
                        case 4:
                            CHECK_SQLITE3(sqlite3_bind_double(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<float>(i))), database_);
                            break;
                        case 8:
                            CHECK_SQLITE3(sqlite3_bind_double(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<double>(i))), database_);
                            break;
                        default:
                            log_fatal("Can't handle floating points of size '%li' (field: '%s').", type.size, description_->GetFieldNames()[i].c_str());
                    }
                    break;
                case I3Datatype::TypeClass::Int:
                case I3Datatype::TypeClass::Enum:
                    if (type.is_signed) {
                        switch (type.size) {
                            case 1:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<int8_t>(i))), database_);
                                break;
                            case 2:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<int16_t>(i))), database_);
                                break;
                            case 4:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<int32_t>(i))), database_);
                                break;
                            case 8:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<int64_t>(i))), database_);
                                break;
                            default:
                                log_fatal("Can't handle signed integers of size '%li' (field: '%s').", type.size, description_->GetFieldNames()[i].c_str());
                        }
                    }
                    else {
                        switch (type.size) {
                            case 1:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<uint8_t>(i))), database_);
                                break;
                            case 2:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<uint16_t>(i))), database_);
                                break;
                            case 4:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<uint32_t>(i))), database_);
                                break;
                            case 8:
                                CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<uint64_t>(i))), database_);
                                break;
                            default:
                                log_fatal("Can't handle unsigned integers of size '%li' (field: '%s').", type.size, description_->GetFieldNames()[i].c_str());
                        }
                    }
                    break;
                case I3Datatype::TypeClass::Bool:
                    CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, i+1-skip_cols_, *(rows->GetPointer<bool>(i))), database_);
                    break;
                default:
                    log_fatal("Can't handle type: '%s' (field: '%s').", type.AsString().c_str(), description_->GetFieldNames()[i].c_str());
            }
        }
        CHECK_SQLITE3(sqlite3_bind_int64(insertion_stmt_, num_fields_+1-skip_cols_, event_no_), database_);
        CHECK_SQLITE3(sqlite3_step(insertion_stmt_), database_);
        CHECK_SQLITE3(sqlite3_reset(insertion_stmt_), database_);
    }
    CHECK_SQLITE3(sqlite3_exec(database_, "COMMIT;", 0, 0, nullptr), database_);
    rows->SetEnumsAreInts(false);
    event_no_++;
}


std::string I3SQLiteTable::GetSQLiteType(const I3Datatype &type) const
{
    switch (type.kind) {
        case I3Datatype::TypeClass::Float:
            return "DOUBLE";
        case I3Datatype::TypeClass::Int:
        case I3Datatype::TypeClass::Enum:
            return "INTEGER";
        case I3Datatype::TypeClass::Bool:
            return "BOOLEAN";
        default:
            return "NULL";
    }
}


