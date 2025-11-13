/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Declaration of I3ParquetTable
 *
 * @file I3ParquetTable.h
 * @date 2025-03-24
 * @author lepeler
 *
*/

#ifndef I3PARQUETTABLE_H_INCLUDED
#define I3PARQUETTABLE_H_INCLUDED


#include <string>
#include <cstdint>

#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <boost/filesystem.hpp>

#include <tableio/I3Table.h>
#include <tableio/I3TableRow.h>
#include <tableio/I3TableService.h>

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);



class I3ParquetTable : public I3Table {

public:
    I3ParquetTable(I3TableService &service,
                   std::string tableName,
                   I3TableRowDescriptionConstPtr description,
                   boost::filesystem::path folder_path,
                   parquet::Compression::type compression);

    virtual ~I3ParquetTable();

protected:
    virtual void WriteRows(I3TableRowConstPtr row);

    void CreateTable();

private:
    boost::filesystem::path folder_path_;
    boost::filesystem::path file_path_;
    parquet::Compression::type compression_;
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> array_builders_;
    std::shared_ptr<arrow::Schema> schema_;
    size_t num_fields_;
    size_t event_no_;
    uint8_t skip_fields_;

    std::shared_ptr<arrow::DataType> GetArrowType(const I3Datatype &type) const;

SET_LOGGER("I3ParquetTable");
};



#endif //I3PARQUETTABLE_H_INCLUDED

