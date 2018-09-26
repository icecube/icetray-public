/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "tableio/textwriter/I3CSVTable.h"
#include "tableio/I3TableRow.h"
#include "tableio/I3TableRowDescription.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

I3CSVTable::I3CSVTable(I3TableService& service, const std::string& name,
                       I3TableRowDescriptionConstPtr description,
                       std::string& folderPath, I3TablePtr index) :
 I3Table(service,name,description), folderPath_(folderPath), 
 output_((folderPath + std::string("/") + name + std::string(".csv")).c_str()) {
     indexTable_ = index;
     CreateTable();
}

I3CSVTable::~I3CSVTable() {
    output_.close();
}

I3CSVTable::NativeType I3CSVTable::GetNativeType(const I3Datatype& dtype) {
    I3CSVTable::NativeType type = UNKNOWN;
    switch (dtype.kind) {
        case I3Datatype::Enum:
            // fall through to integers
        case I3Datatype::Int:
            if (dtype.size == sizeof(char)) {
                if (dtype.is_signed) type = NATIVE_SCHAR;
                else                 type = NATIVE_UCHAR;
            } else if (dtype.size == sizeof(short)) {
                if (dtype.is_signed) type = NATIVE_SHORT;
                else                 type = NATIVE_USHORT;
            } else if (dtype.size == sizeof(int)) {
                if (dtype.is_signed) type = NATIVE_INT;
                else                 type = NATIVE_UINT;
            } else if (dtype.size == sizeof(long)) {
                if (dtype.is_signed) type = NATIVE_LONG;
                else                 type = NATIVE_ULONG;
            } else if (dtype.size == sizeof(long long)) {
                if (dtype.is_signed) type = NATIVE_LLONG;
                else                 type = NATIVE_ULLONG;
            }
            break;
        case I3Datatype::Float:
            if (dtype.size == sizeof(float)) {
                type = NATIVE_FLOAT;
            } else if (dtype.size == sizeof(double)) {
                type = NATIVE_DOUBLE;
            }
            break;
        case I3Datatype::Bool:
            type = NATIVE_BOOL;
            break;
    }
    return type;
}


void I3CSVTable::CreateTable() {
    std::vector<std::string>::const_iterator name_it,unit_it,doc_it;
    std::vector<size_t>::const_iterator size_it;
    std::vector<I3Datatype>::const_iterator dtype_it;
    const std::vector<std::string>& names = description_->GetFieldNames();
    const std::vector<std::string>& units = description_->GetFieldUnits();
    const std::vector<std::string>& doc = description_->GetFieldDocStrings();
    const std::vector<size_t>& arrayLengths = description_->GetFieldArrayLengths();
    const std::vector<I3Datatype>& dtypes = description_->GetFieldTypes();
    
    for (name_it = names.begin(), unit_it = units.begin(), size_it = arrayLengths.begin(); 
         name_it != names.end(); 
         name_it++, unit_it++, size_it++) {
        output_ << "\"" << *name_it;
        if (unit_it->size() > 0) output_ << " [" << *unit_it << "]";
        output_ << "\"";
        if (*size_it > 1) {
            for (size_t i = 1; i < *size_it; i++) output_ << ",";
        }
        if (name_it == names.end()-1) {
            output_ << std::endl;
        } else {
            output_ << ",";
        }
    }
    for (doc_it = doc.begin(), size_it = arrayLengths.begin(), dtype_it = dtypes.begin();
         doc_it != doc.end();
         doc_it++, size_it++, dtype_it++) {
        output_ << "\"" << *doc_it;
        // if this is an enum type, write out the enum members
        if (dtype_it->kind == I3Datatype::Enum) {
            output_ << "{ ";
            std::vector<std::pair<std::string,long> >::const_iterator member_it;
            for (member_it = dtype_it->enum_members.begin(); 
                 member_it != dtype_it->enum_members.end(); 
                 member_it++) {
                output_ << member_it->first << " = " << member_it->second;
                if (member_it != dtype_it->enum_members.end()-1) output_ << ", ";
            }
            output_ << " }";
        }
        output_ << "\"";
        // if this is an array type, add enough columns for the array size
        if (*size_it > 1) {
            for (size_t i = 1; i < *size_it; i++) output_ << ",";
        }
        // terminate or continue the line
        if (doc_it == doc.end()-1) {
            output_ << std::endl;
        } else {
            output_ << ",";
        }
    }
    
    output_.precision(12);
    output_ << std::scientific;
    
    for (dtype_it = dtypes.begin(); dtype_it != dtypes.end(); dtype_it++) {
        fieldTypes_.push_back(GetNativeType(*dtype_it));
    }
}

void I3CSVTable::WriteRows(I3TableRowConstPtr row) {
    unsigned int i,j;
    std::vector<NativeType>::iterator t_it;
    I3TableRowPtr rows = boost::const_pointer_cast<I3TableRow>(row);
    
    const std::vector<size_t>& arrayLengths = description_->GetFieldArrayLengths();
    std::vector<size_t>::const_iterator size_it;
    rows->SetEnumsAreInts(true);
    for (i = 0; i < rows->GetNumberOfRows(); i++) {
        rows->SetCurrentRow(i);
        for (t_it = fieldTypes_.begin(), size_it = arrayLengths.begin(), j = 0;
             t_it != fieldTypes_.end();
             t_it++, size_it++, j++) {
            switch (*t_it) {
                case NATIVE_SCHAR:
                    WriteField(rows->GetPointer<signed char>(j),*size_it);
                    break;
                case NATIVE_UCHAR:
                    WriteField(rows->GetPointer<unsigned char>(j),*size_it);
                    break;
                case NATIVE_SHORT:
                    WriteField(rows->GetPointer<short>(j),*size_it);
                    break;
                case NATIVE_USHORT:
                    WriteField(rows->GetPointer<unsigned short>(j),*size_it);
                    break;
                case NATIVE_INT:
                    WriteField(rows->GetPointer<int>(j),*size_it);
                    break;
                case NATIVE_UINT:
                    WriteField(rows->GetPointer<unsigned int>(j),*size_it);
                    break;
                case NATIVE_LONG:
                    WriteField(rows->GetPointer<long>(j),*size_it);
                    break;
                case NATIVE_ULONG:
                    WriteField(rows->GetPointer<unsigned long>(j),*size_it);
                    break;
                case NATIVE_LLONG:
                    WriteField(rows->GetPointer<long long>(j),*size_it);
                    break;
                case NATIVE_ULLONG:
                    WriteField(rows->GetPointer<unsigned long long>(j),*size_it);
                    break;
                case NATIVE_FLOAT:
                    WriteField(rows->GetPointer<float>(j),*size_it);
                    break;
                case NATIVE_DOUBLE:
                    WriteField(rows->GetPointer<double>(j),*size_it);
                    break;
                case NATIVE_BOOL:
                    WriteField(rows->GetPointer<bool>(j),*size_it);
                    break;
                default:
                    log_warn("Don't know how to write type '%s'",description_->GetFieldTypes().at(j).description.c_str());
            }
            if (t_it == fieldTypes_.end()-1) {
                output_ << std::endl;
            } else {
                output_ << ",";
            }
        }
    }
    rows->SetEnumsAreInts(false);
}
