/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Implementation of I3ParquetTableService
 *
 * @file I3ParquetTableService.h
 * @date 2025-03-24
 * @author lepeler
 *
*/

#include <string>
#include <cstdint>

#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <boost/filesystem.hpp>

#include <tableio/I3Table.h>
#include <tableio/I3TableRow.h>
#include <tableio/I3TableService.h>

#include "I3ParquetTable.h"
#include "I3ParquetTableService.h"
#include "parquet_utils.h"



I3ParquetTableService::I3ParquetTableService(boost::filesystem::path folder_path,
                                             std::string compression = "uncompressed")
                                             : I3TableService()
{
    folder_path_ = folder_path;
    boost::filesystem::create_directory(folder_path_);
    compression_ = get_compression_type(compression);
}

I3ParquetTableService::~I3ParquetTableService()
{}

I3TablePtr I3ParquetTableService::CreateTable(const std::string &tableName,
                                              I3TableRowDescriptionConstPtr description)
{
    return I3TablePtr(new I3ParquetTable(*this, tableName, description, folder_path_, compression_));
}

void I3ParquetTableService::CloseFile()
{}


