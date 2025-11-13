/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Declaration of I3ParquetTableService
 *
 * @file I3ParquetTableService.h
 * @date 2025-03-24
 * @author lepeler
 *
*/

#ifndef	I3PARQUETTABLESERVICE_H_INCLUDED
#define I3PARQUETTABLESERVICE_H_INCLUDED

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



class I3ParquetTableService : public I3TableService {

public:
    I3ParquetTableService(boost::filesystem::path folder_path,
                          std::string compression);

    virtual ~I3ParquetTableService();

protected:
    virtual I3TablePtr CreateTable(const std::string &tableName,
                                   I3TableRowDescriptionConstPtr description);

    virtual void CloseFile();

private:
    boost::filesystem::path folder_path_;
    parquet::Compression::type compression_;
};



#endif //I3PARQUETTABLESERVICE_H_INCLUDED

