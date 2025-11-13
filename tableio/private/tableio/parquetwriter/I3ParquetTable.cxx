/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Implementation of I3ParquetTable
 *
 * @file I3ParquetTable.h
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
#include "parquet_utils.h"



I3ParquetTable::I3ParquetTable(I3TableService &service,
                               std::string tableName,
                               I3TableRowDescriptionConstPtr description,
                               boost::filesystem::path folder_path,
                               parquet::Compression::type compression = parquet::Compression::UNCOMPRESSED)
                               : I3Table(service,
                                         tableName,
                                         description)
{
    folder_path_ = folder_path;
    compression_ = compression;
    file_path_ = folder_path_ / (name_ + ".parquet" + get_compression_file_ending(compression_));

    num_fields_ = description_->GetFieldNames().size();
    event_no_ = 0;
    if (tableName == "I3EventHeader")
        {skip_fields_ = 0;}
    else
        {skip_fields_ = 5;}

    CreateTable();
}

I3ParquetTable::~I3ParquetTable()
{
    // create arrays from the builders
    std::vector<std::shared_ptr<arrow::Array>> arrays = get_arrays(array_builders_);
    // open parquet file
    std::shared_ptr<arrow::io::FileOutputStream> out_file;
    PARQUET_ASSIGN_OR_THROW(out_file, arrow::io::FileOutputStream::Open(file_path_.string()));

    // write the arrays to the parquet file
    write_table(out_file, schema_, arrays, compression_);

    // close parquet file
    PARQUET_THROW_NOT_OK(out_file->Close());
}

void I3ParquetTable::CreateTable()
{
    // get parquet names and types of columns
    std::vector<std::string> field_names = description_->GetFieldNames();
    std::vector<std::shared_ptr<arrow::DataType>> field_types;
    for (const I3Datatype & type : description_->GetFieldTypes()) {
        field_types.push_back(GetArrowType(type));
    }

    // check that all a correspondong arrow::DataType was found for all I3Datatypes 
    bool all_types_fine = true;
    for (size_t i = 0; i < field_types.size(); i++) {
        if (field_types[i] == arrow::null()) {
            log_warn("Can't handle type: '%s' (field: '%s').", description_->GetFieldTypes()[i].AsString().c_str(), description_->GetFieldNames()[i].c_str());
            all_types_fine = false;
        }
    }
    if (!all_types_fine) {
        log_fatal("For some I3Datatypes no corresponding SQLite arrow::DataType was found.");
    }

    // skip first skip_fields_ fields
    field_names.erase(field_names.begin(), field_names.begin()+skip_fields_);
    field_types.erase(field_types.begin(), field_types.begin()+skip_fields_);

    // create arrow schema and array builders
    array_builders_ = get_array_builders(field_types);
    schema_ = get_schema(field_names, field_types);
}

void I3ParquetTable::WriteRows(I3TableRowConstPtr row)
{
    I3TableRowPtr rows = boost::const_pointer_cast<I3TableRow>(row);
    rows->SetEnumsAreInts(true);
    I3Datatype type;
    // insert data
    for (size_t n = 0; n < rows->GetNumberOfRows(); n++) {
        rows->SetCurrentRow(n);
        for (size_t i = skip_fields_; i < num_fields_; i++) {
            type = description_->GetFieldTypes()[i];
            switch (type.kind) {
                case I3Datatype::TypeClass::Float:
                    switch (type.size) {
                        case 4:
                            PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::FloatBuilder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<float>(i))));
                            break;
                        case 8:
                            PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::DoubleBuilder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<double>(i))));
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
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::Int8Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<int8_t>(i))));
                                break;
                            case 2:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::Int16Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<int16_t>(i))));
                                break;
                            case 4:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::Int32Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<int32_t>(i))));
                                break;
                            case 8:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::Int64Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<int64_t>(i))));
                                break;
                            default:
                                log_fatal("Can't handle signed integers of size '%li' (field: '%s').", type.size, description_->GetFieldNames()[i].c_str());
                        }
                    }
                    else {
                        switch (type.size) {
                            case 1:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::UInt8Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<uint8_t>(i))));
                                break;
                            case 2:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::UInt16Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<uint16_t>(i))));
                                break;
                            case 4:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::UInt32Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<uint32_t>(i))));
                                break;
                            case 8:
                                PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::UInt64Builder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<uint64_t>(i))));
                                break;
                            default:
                                log_fatal("Can't handle unsigned integers of size '%li' (field: '%s').", type.size, description_->GetFieldNames()[i].c_str());
                        }
                    }
                    break;
                case I3Datatype::TypeClass::Bool:
                    PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::BooleanBuilder>(array_builders_[i-skip_fields_])->Append(*(rows->GetPointer<bool>(i))));
                    break;
                default:
                    log_fatal("Can't handle type: '%s' (field: '%s').", type.AsString().c_str(), description_->GetFieldNames()[i].c_str());
            }
        }
        PARQUET_THROW_NOT_OK(std::static_pointer_cast<arrow::UInt64Builder>(array_builders_[num_fields_-skip_fields_])->Append(event_no_));
    }
    rows->SetEnumsAreInts(false);
    event_no_++;
}

std::shared_ptr<arrow::DataType> I3ParquetTable::GetArrowType(const I3Datatype &type) const
{
    switch (type.kind) {
        case I3Datatype::TypeClass::Float:
            switch (type.size) {
                case 2:
                    return arrow::float16();
                case 4:
                    return arrow::float32();
                case 8:
                    return arrow::float64();
                default:
                    return arrow::null();
            }
        case I3Datatype::TypeClass::Int:
        case I3Datatype::TypeClass::Enum:
            if (type.is_signed) {
                switch (type.size) {
                    case 1:
                        return arrow::int8();
                    case 2:
                        return arrow::int16();
                    case 4:
                        return arrow::int32();
                    case 8:
                        return arrow::int64();
                    default:
                        return arrow::null();
                }
            }
            else {
                switch (type.size) {
                    case 1:
                        return arrow::uint8();
                    case 2:
                        return arrow::uint16();
                    case 4:
                        return arrow::uint32();
                    case 8:
                        return arrow::uint64();
                    default:
                        return arrow::null();
                }
            }
        case I3Datatype::TypeClass::Bool:
            return arrow::boolean();
        default:
            return arrow::null();
    }
}


