/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Implementation of I3SQLiteTableService
 *
 * @file I3SQLiteTableService.cxx
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
#include "I3SQLiteTableService.h"



I3SQLiteTableService::I3SQLiteTableService(boost::filesystem::path path)
                                           : I3TableService()
{
    path_ = path;
}

I3SQLiteTableService::~I3SQLiteTableService()
{}

I3TablePtr I3SQLiteTableService::CreateTable(const std::string &tableName,
                                             I3TableRowDescriptionConstPtr description)
{
    return I3TablePtr(new I3SQLiteTable(*this, tableName, description, path_));
}

void I3SQLiteTableService::CloseFile()
{}


