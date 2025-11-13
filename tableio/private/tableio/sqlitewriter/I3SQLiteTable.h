/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Declaration of I3SQLiteTable
 *
 * @file I3SQLiteTable.h
 * @date 2025-02-16
 * @author lepeler
 *
*/

#ifndef I3SQLITETABLE_H_INCLUDED
#define I3SQLITETABLE_H_INCLUDED


#include <string>
#include <cstdint>

#include <sqlite3.h>
#include <boost/filesystem.hpp>

#include <tableio/I3Table.h>
#include <tableio/I3TableRow.h>
#include <tableio/I3TableService.h>

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);



class I3SQLiteTable : public I3Table {

public:
    I3SQLiteTable(I3TableService &service,
                  std::string tableName,
                  I3TableRowDescriptionConstPtr description,
                  boost::filesystem::path path);

    virtual ~I3SQLiteTable();

protected:
    virtual void WriteRows(I3TableRowConstPtr row);

    void CreateTable();

private:
    boost::filesystem::path db_path_;
    sqlite3 *database_;
    sqlite3_stmt *insertion_stmt_;
    size_t num_fields_;
    size_t event_no_;
    uint8_t skip_cols_;

    std::string GetSQLiteType(const I3Datatype &type) const;

SET_LOGGER("I3SQLiteTable");
};



#endif //I3SQLITETABLE_H_INCLUDED

