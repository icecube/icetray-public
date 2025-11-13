/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Declaration of I3SQLiteTableService
 *
 * @file I3SQLiteTableService.h
 * @date 2025-02-16
 * @author lepeler
 *
*/

#ifndef	I3SQLITETABLESERVICE_H_INCLUDED
#define I3SQLITETABLESERVICE_H_INCLUDED

#include <string>
#include <cstdint>

#include <sqlite3.h>
#include <boost/filesystem.hpp>

#include <tableio/I3Table.h>
#include <tableio/I3TableRow.h>
#include <tableio/I3TableService.h>

#include "I3SQLiteTable.h"



class I3SQLiteTableService : public I3TableService {

public:
    I3SQLiteTableService(boost::filesystem::path path);

    virtual ~I3SQLiteTableService();

protected:
    virtual I3TablePtr CreateTable(const std::string &tableName,
                                   I3TableRowDescriptionConstPtr description);

    virtual void CloseFile();

private:
    boost::filesystem::path path_;
};



#endif //I3SQLITETABLESERVICE_H_INCLUDED

