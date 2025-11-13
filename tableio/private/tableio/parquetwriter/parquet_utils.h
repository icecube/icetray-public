/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Collection of utils functions for Parquet file operations
 *
 * @file parquet_utils.h
 * @date 2025-06-13
 * @author lepeler
 *
*/

#ifndef PARQUET_UTILS_H_INCLUDED
#define PARQUET_UTILS_H_INCLUDED


#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

#include <arrow/api.h>
#include <arrow/io/file.h>
#include <arrow/compute/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/arrow/reader.h>
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

    std::map<U, size_t> elem_counts;
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
size_t get_index(T elem, const std::vector<T> &vec) {
    // get the index of elem in vec,
    // if it isn't present return vec.size()

    return std::distance(vec.begin(), std::find(vec.begin(), vec.end(), elem));
}


inline
std::pair<std::vector<std::string>, std::vector<std::shared_ptr<arrow::DataType>>> get_fields(const std::shared_ptr<arrow::Schema> &schema) {
    // get the field names and datatypes from the arrow schema (without event_no)

    std::vector<std::string> field_names;
    std::vector<std::shared_ptr<arrow::DataType>> field_types;

    for (std::shared_ptr<arrow::Field> field : schema->fields()) {
        if (field->name() != "event_no") {
            field_names.push_back(field->name());
            field_types.push_back(field->type());
        }
    }

    return std::make_pair(field_names, field_types);
}


inline
std::shared_ptr<arrow::ArrayBuilder> get_array_builder(const std::shared_ptr<arrow::DataType> &type) {
    // get the array builder corresponding to an arrow datatype

    std::shared_ptr<arrow::ArrayBuilder> array_builder;
    PARQUET_ASSIGN_OR_THROW(array_builder, arrow::MakeBuilder(type, arrow::default_memory_pool()));

    return array_builder;
}


inline
std::vector<std::shared_ptr<arrow::ArrayBuilder>> get_array_builders(const std::vector<std::shared_ptr<arrow::DataType>> &field_types) {
    // get arrow array builders (add event_no to specified fields)

    std::vector<std::shared_ptr<arrow::ArrayBuilder>> array_builders;
    for (std::shared_ptr<arrow::DataType> field_type : field_types) {
        array_builders.push_back(get_array_builder(field_type));
    }
    array_builders.push_back(std::make_shared<arrow::UInt64Builder>());

    return array_builders;
}


inline
std::shared_ptr<arrow::Schema> get_schema(const std::vector<std::string> &field_names, const std::vector<std::shared_ptr<arrow::DataType>> &field_types) {
    // get arrow schema (add event_no to specified fields)

    std::vector<std::shared_ptr<arrow::Field>> fields;
    for (size_t i = 0; i < field_names.size(); i++) {
        fields.push_back(std::make_shared<arrow::Field>(field_names[i], field_types[i]));
    }
    fields.push_back(std::make_shared<arrow::Field>("event_no", arrow::uint64()));
    
    return std::make_shared<arrow::Schema>(fields);
}


inline
std::vector<std::shared_ptr<arrow::Array>> get_arrays(const std::vector<std::shared_ptr<arrow::ArrayBuilder>> &array_builders) {
    // get the arrow arrays from the array builders

    std::vector<std::shared_ptr<arrow::Array>> arrays;
    std::shared_ptr<arrow::Array> array_tmp;
    for (std::shared_ptr<arrow::ArrayBuilder> array_builder : array_builders) {
        PARQUET_THROW_NOT_OK(array_builder->Finish(&array_tmp));
        arrays.push_back(array_tmp);
    }

    return arrays;
}


inline
parquet::Compression::type get_compression_type(std::string compression) {
    // get the parquet compression type from the string

    if (compression == "brotli" || compression == ".br")
        {return parquet::Compression::BROTLI;}
    else if (compression == "bz2" || compression == ".bz2")
        {return parquet::Compression::BZ2;}
    else if (compression == "gzip" || compression == ".gz")
        {return parquet::Compression::GZIP;}
    else if (compression == "lz4" || compression == ".lz4")
        {return parquet::Compression::LZ4;}
    else if (compression == "lz4_frame")
        {return parquet::Compression::LZ4_FRAME;}
    else if (compression == "lz4_hadoop")
        {return parquet::Compression::LZ4_HADOOP;}
    else if (compression == "lz0" || compression == ".lz0")
        {return parquet::Compression::LZO;}
    else if (compression == "snappy" || compression == ".sz")
        {return parquet::Compression::SNAPPY;}
    else if (compression == "uncompressed" || compression == "")
        {return parquet::Compression::UNCOMPRESSED;}
    else if (compression == "zstd" || compression == ".zst")
        {return parquet::Compression::ZSTD;}
    else {
        throw std::runtime_error("The compression type " + compression + " is currently not supported.");
    }
}


inline
std::string get_compression_file_ending(const parquet::Compression::type &compression) {
    // get the file ending for the parquet compression type

    switch (compression) {
        case parquet::Compression::BROTLI:
            return ".br";
        case parquet::Compression::BZ2:
            return ".bz2";
        case parquet::Compression::GZIP:
            return ".gz";
        case parquet::Compression::LZ4:
        case parquet::Compression::LZ4_FRAME:
        case parquet::Compression::LZ4_HADOOP:
            return ".lz4";
        case parquet::Compression::LZO:
            return ".lz0";
        case parquet::Compression::SNAPPY:
            return ".sz";
        case parquet::Compression::UNCOMPRESSED:
            return "";
        case parquet::Compression::ZSTD:
            return ".zst";
        default:
            throw std::runtime_error("The specified compression type doesn't exist or is unsupported (enum value: " + std::to_string(static_cast<uint8_t>(compression)) + ").");
    }
}


inline
void write_table(const std::shared_ptr<arrow::io::FileOutputStream> &out_file, const std::shared_ptr<arrow::Schema> &schema, const std::vector<std::shared_ptr<arrow::Array>> &arrays, const parquet::Compression::type &compression) {
    // write an arrow table to a parquet file

    // create arrow table
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, arrays);

    // set parquet writer and writer properties
    parquet::WriterProperties::Builder writer_props_builder;
    writer_props_builder.compression(compression);
    parquet::ArrowWriterProperties::Builder arrow_writer_props_builder;
    std::shared_ptr<parquet::arrow::FileWriter> writer;
    PARQUET_ASSIGN_OR_THROW(writer, parquet::arrow::FileWriter::Open(*schema, arrow::default_memory_pool(), out_file, writer_props_builder.build(), arrow_writer_props_builder.build()));
    // write the table to the parquet file and close writer
    PARQUET_THROW_NOT_OK(writer->WriteTable(*table, table->num_rows()));
    PARQUET_THROW_NOT_OK(writer->Close());
}


inline
uint64_t get_event_no_offset(const std::shared_ptr<parquet::arrow::FileReader> &reader, int event_no_index) {
    // get the maximum event_no from the reader plus one

    if (reader->num_row_groups() == 0) {
        return 0;
    }

    uint64_t event_no_max = 0;

    std::shared_ptr<arrow::Table> tmp_table;
    std::shared_ptr<arrow::UInt64Array> tmp_array;
    // iterate over the individual row groups and compare all event_nos to the current maximum
    for (int n = 0; n < reader->num_row_groups(); n++) {
        // read the event_no_array of the row group
        PARQUET_THROW_NOT_OK(reader->ReadRowGroup(n, {event_no_index}, &tmp_table));
        tmp_array = std::static_pointer_cast<arrow::UInt64Array>(tmp_table->column(0)->chunks()[0]);

        // get its maximum
        for (int64_t i = 0; i < tmp_array->length(); i++) {
            event_no_max = std::max(tmp_array->Value(i), event_no_max);
        }
    }

    return event_no_max + 1;
}


inline
arrow::Status offset_event_no_column(std::shared_ptr<arrow::Table> &table, int event_no_index, uint64_t offset) {
    // offset the table's event_no column by offset

    std::vector<std::shared_ptr<arrow::ChunkedArray>> columns = table->columns();
    std::vector<std::shared_ptr<arrow::Array>> chunks = columns.at(event_no_index)->chunks();

    arrow::UInt64Builder builder;
    std::shared_ptr<arrow::UInt64Array> tmp_array;
    // iterate over the chunks and offset all their elements, creating new arrays with them
    for (std::shared_ptr<arrow::Array> &chunk : chunks) {
        tmp_array = std::static_pointer_cast<arrow::UInt64Array>(chunk);

        // offset all elements and append them to the array builder
        for (int64_t i = 0; i < tmp_array->length(); i++) {
            PARQUET_THROW_NOT_OK(builder.Append(tmp_array->Value(i) + offset));
        }
        PARQUET_THROW_NOT_OK(builder.Finish(&chunk));
        builder.Reset();
    }

    // replace column at index with the offset one and then create a new table with it
    columns.at(event_no_index) = std::make_shared<arrow::ChunkedArray>(chunks);
    table = arrow::Table::Make(table->schema(), columns);

    return arrow::Status::OK();
}


inline
bool all_chunked_arrays_equal_length(const std::vector<std::shared_ptr<arrow::ChunkedArray>> &array_vec) {
    // are all chunked arrays in the vector of equal length?

    if (array_vec.empty())
        {return true;}

    for (std::shared_ptr<arrow::ChunkedArray> array : array_vec) {
        if (array->length() != array_vec.back()->length())
            {return false;}
    }
    return true;
}


inline
bool all_chunked_arrays_equal(const std::vector<std::shared_ptr<arrow::ChunkedArray>> &array_vec) {
    // are all chunked arrays in the vector equal?

    if (array_vec.empty())
        {return true;}

    for (std::shared_ptr<arrow::ChunkedArray> array : array_vec) {
        if (!array->Equals(array_vec.back()))
            {return false;}
    }
    return true;
}



inline
std::pair<std::shared_ptr<arrow::io::FileOutputStream>, std::shared_ptr<parquet::arrow::FileWriter>> Open_as_Append(std::string path, parquet::Compression::type &compression) {
    // open a parquet file in append mode
    // the arrow api doesn't implement this natively
    // so here is a sadly inefficient workaround by renaming the file,
    // opening a new one under the original filename and copying the original contents into it

    boost::filesystem::rename(path, path + ".copy");

    // open copied file
    std::shared_ptr<arrow::io::ReadableFile> file_read;
    std::shared_ptr<parquet::arrow::FileReader> reader;
    std::shared_ptr<arrow::Schema> schema;
    PARQUET_ASSIGN_OR_THROW(file_read, arrow::io::ReadableFile::Open(path + ".copy"));
    PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(file_read, arrow::default_memory_pool()));
    PARQUET_THROW_NOT_OK(reader->GetSchema(&schema));

    // open append file
    std::shared_ptr<arrow::io::FileOutputStream> file_append;
    PARQUET_ASSIGN_OR_THROW(file_append, arrow::io::FileOutputStream::Open(path));
    parquet::WriterProperties::Builder writer_props_builder;
    writer_props_builder.compression(compression);
    parquet::ArrowWriterProperties::Builder arrow_writer_props_builder;
    std::shared_ptr<parquet::arrow::FileWriter> writer;
    PARQUET_ASSIGN_OR_THROW(writer, parquet::arrow::FileWriter::Open(*schema, arrow::default_memory_pool(), file_append, writer_props_builder.build(), arrow_writer_props_builder.build()));

    // loop over all row groups in the copied file and insert into the append file
    std::vector<std::shared_ptr<arrow::ChunkedArray>> columns;
    std::shared_ptr<arrow::Table> table;

    for (int n = 0; n < reader->num_row_groups(); n++) {
        // read the row group
        PARQUET_THROW_NOT_OK(reader->RowGroup(n)->ReadTable(&table));
        // and write the table to the file append
        PARQUET_THROW_NOT_OK(writer->WriteTable(*table, table->num_rows()));
    }

    // close file read and delete it
    reader.reset();
    PARQUET_THROW_NOT_OK(file_read->Close());

    boost::filesystem::remove(path + ".copy");

    return std::make_pair(file_append, writer);
}



#endif //PARQUET_UTILS_H_INCLUDED

