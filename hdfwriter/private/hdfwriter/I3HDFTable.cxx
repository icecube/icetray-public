/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#include <assert.h>

#include "hdfwriter/I3HDFTable.h"
#include "tableio/I3Converter.h"
#include "tableio/I3TableRow.h"
#include "tableio/I3TableRowDescription.h"

#include "H5Tpublic.h"

#if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
	#include "hdf5_hl.h"
#else
	#include "H5TA.h"
#endif

#include "H5Dpublic.h"
#include "H5Gpublic.h"

// customize table creation
#include "hdf5_opt.h"

/******************************************************************************/

// If no description provided, read one from the table on disk
I3HDFTable::I3HDFTable(I3TableService& service, const std::string& name,
                       hid_t fileId, I3TablePtr index_table) :
    I3Table(service, name, I3TableRowDescriptionPtr()),
    fileId_(fileId) {
      indexTable_ = index_table;
      if (indexTable_) nevents_ = indexTable_->GetNumberOfRows();
      else nevents_ = 0;
      CreateDescription();
      CalculateChunkSize();
      CreateCache();
}

/******************************************************************************/

// If a description is provided, create the corresponding table
I3HDFTable::I3HDFTable(I3TableService& service, const std::string& name,
                       I3TableRowDescriptionConstPtr description,
                       hid_t fileId, int compress, I3TablePtr index_table) :
    I3Table(service, name, description),
    fileId_(fileId) {
      indexTable_ = index_table;
      CalculateChunkSize();
      CreateTable(compress);
      CreateCache();
}

/******************************************************************************/

void I3HDFTable::CreateCache() {
    writeCache_ = I3TableRowPtr(new I3TableRow(description_,0));
    writeCache_->reserve(CHUNKTIMES*chunkSize_);
}


/******************************************************************************/

// Calculate the number of rows in a data chunk
void I3HDFTable::CalculateChunkSize() {
    size_t byteSize = 0;
    const std::vector<I3Datatype>& fieldI3Datatypes = description_->GetFieldTypes();
    const std::vector<size_t>& fieldArrayLengths = description_->GetFieldArrayLengths();
    std::vector<I3Datatype>::const_iterator it;
    std::vector<size_t>::const_iterator as_it;
    
    for (it = fieldI3Datatypes.begin(), as_it = fieldArrayLengths.begin();
         it != fieldI3Datatypes.end();
         it++, as_it++)
      byteSize += (it->size)*(*as_it);
    if (byteSize == 0)
      log_fatal("Cowardly refusing to divide by zero!");
    chunkSize_ = size_t(CHUNKSIZE_BYTES)/byteSize;
    log_trace("%s Chunk shape is %zu",log_label().c_str(),chunkSize_);
}

/******************************************************************************/

// construct an HDF type from an I3Datatype
hid_t I3HDFTable::GetHDFType(const I3Datatype& dtype, const size_t arrayLength) {
    hid_t hdftype=0;
    std::vector<std::pair<std::string,long> >::const_iterator enum_it;
    switch (dtype.kind) {
        case I3Datatype::Bool:
            // fall through to integers (just like C, there is no native HDF5 bool)
        case I3Datatype::Int:
            hdftype = H5Tcopy(H5T_NATIVE_INT);
            H5Tset_size(hdftype,dtype.size);
            H5Tset_precision(hdftype,8*dtype.size);
            if (dtype.is_signed) H5Tset_sign(hdftype,H5T_SGN_2);
            else H5Tset_sign(hdftype,H5T_SGN_NONE);
            break;
        case I3Datatype::Float:
            if (dtype.size == sizeof(float)) {
                hdftype = H5Tcopy(H5T_NATIVE_FLOAT);
            } else if (dtype.size == sizeof(double)) {
                hdftype = H5Tcopy(H5T_NATIVE_DOUBLE);
            } else if (dtype.size == sizeof(long double)) {
                hdftype = H5Tcopy(H5T_NATIVE_LDOUBLE);
            } else {
                log_fatal("I don't know what do with %zu-byte floats.",dtype.size);
            }
            break;
        case I3Datatype::Enum:
            {
                hdftype = H5Tcopy(H5T_NATIVE_INT);
                H5Tset_size(hdftype,dtype.size);
                H5Tset_precision(hdftype,8*dtype.size);
                if (dtype.is_signed) H5Tset_sign(hdftype,H5T_SGN_2);
                else H5Tset_sign(hdftype,H5T_SGN_NONE);
                hid_t enumtype = H5Tenum_create(hdftype);
                H5Tclose(hdftype);
                hdftype = enumtype;
                for (enum_it = dtype.enum_members.begin(); enum_it != dtype.enum_members.end(); enum_it++) {
                    H5Tenum_insert(hdftype,enum_it->first.c_str(),&(enum_it->second));
                }
            }
            break;     
        default:
            log_fatal("I don't know what do with datatype %d.",dtype.kind);
            break;     
    }
    if (arrayLength > 1) {
        hsize_t rank = 1; 
        std::vector<hsize_t> dims(1, arrayLength);
        hid_t array_tid = H5Tarray_create(hdftype, rank, &dims.front(), NULL);
        H5Tclose(hdftype);
        hdftype = array_tid;
    }
    return hdftype;
}

/******************************************************************************/

// construct an I3Datatype from an HDF type
I3Datatype I3HDFTable::GetI3Datatype(hid_t hdftype, size_t* arrayLength ) {
    I3Datatype dtype;
    char* name;
    long value;
    hsize_t i,n,array_size;
    int rank,perm;
    
    switch (H5Tget_class(hdftype)) {
        case H5T_INTEGER:
            dtype.kind = I3Datatype::Int;
            dtype.size = H5Tget_size(hdftype);
            if (H5Tget_sign(hdftype) == H5T_SGN_2) dtype.is_signed = true;
            else dtype.is_signed = false;
            break;
        case H5T_FLOAT:
            dtype.kind = I3Datatype::Float;
            dtype.size = H5Tget_size(hdftype);
            dtype.is_signed = true;
            break;
        case H5T_ENUM:
            dtype.kind = I3Datatype::Enum;
            dtype.size = H5Tget_size(hdftype);
            dtype.is_signed = (H5Tget_sign(hdftype) == H5T_SGN_2);
            n = H5Tget_nmembers(hdftype);
            dtype.enum_members.reserve(n);
            for (i = 0; i < n; i++) {
                name = H5Tget_member_name(hdftype,i);
                H5Tget_member_value(hdftype,i,&value);
                dtype.enum_members.push_back(std::make_pair(std::string(name),value));
                free(name);
            }
            break;
        case H5T_ARRAY:
            dtype = GetI3Datatype(H5Tget_super(hdftype),NULL);
            rank = H5Tget_array_ndims(hdftype);
            if (rank != 1) log_fatal("Rank of array is %d. This is deeply screwy!",rank);
            H5Tget_array_dims(hdftype,&array_size,&perm);
            if(arrayLength != NULL)
              *arrayLength = size_t(array_size);
            else
              log_fatal("Cowardly refusing to work with a NULL pointer");
            break;
        default:
            log_fatal("Unknown HDF type class %d.",(int)H5Tget_class(hdftype));
            break;
    }
    return dtype;
}

/******************************************************************************/

// create a table based on the TableRowDescription
void I3HDFTable::CreateTable(int& compress) {

    const size_t structSize = description_->GetTotalByteSize();
    const unsigned nfields = description_->GetNumberOfFields();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    
    // build HDF types from the supplied I3Datatypes
    std::vector<hid_t> fieldHdfTypes;
    std::vector<I3Datatype>::const_iterator t_it;
    std::vector<size_t>::const_iterator arraySize_it;
    const std::vector<I3Datatype>& fieldI3Datatypes = description_->GetFieldTypes();
    const std::vector<size_t>& fieldArrayLengths = description_->GetFieldArrayLengths();
    for (t_it = fieldI3Datatypes.begin(), arraySize_it = fieldArrayLengths.begin();
         t_it != fieldI3Datatypes.end(); 
         t_it++, arraySize_it++) {
             fieldHdfTypes.push_back(GetHDFType(*t_it,*arraySize_it));
    }
    const hid_t* fieldTypes    = &(fieldHdfTypes.front());
  
    // convert field name strings into char**
    std::vector<const char*> fieldNameVector;
    const std::vector<std::string>& nameStrings = description_->GetFieldNames();
    std::vector<std::string>::const_iterator iter;
    for (iter = nameStrings.begin(); iter != nameStrings.end(); ++iter) {
        fieldNameVector.push_back(iter->c_str());
    }
    const char** fieldNames = static_cast<const char**>(&fieldNameVector.front());

    herr_t status = 
        I3H5TBmake_table("",                // table title
                       fileId_,             // hdf5 file opened by writer service 
                       name_.c_str(),       // name of the data set
                       nfields,             // number of table fiels
                       0,                   // number of records writen at creation
                       structSize,          // size of the struct
                       fieldNames,          // table field names
                       fieldOffsets,        // struct offsets
                       fieldTypes,          // field types
                       (hsize_t)chunkSize_, // write data in chunks of ...
                       NULL,                // fill data to be written at creation
                       compress,            // compression level
                       NULL);               // data to be written at creation  
    if (status < 0) {
        log_fatal("Couln't create table");
    }
    else {
        tableCreated_ = true;
    }

   // Add some helpful attributes
   const std::vector<std::string>& unitStrings = description_->GetFieldUnits();
   const std::vector<std::string>& docStrings  = description_->GetFieldDocStrings();
   std::vector<std::string>::const_iterator it_unit;
   std::vector<std::string>::const_iterator it_doc;
   unsigned int i;
   for (i=0, it_unit = unitStrings.begin(), it_doc = docStrings.begin();
        it_unit != unitStrings.end();
        it_unit++, it_doc++, i++) {
         
         std::ostringstream osu,osd;
         // build attribute names
         osu << "FIELD_" << i << "_UNIT";
         osd << "FIELD_" << i << "_DOC";

         // set string attributes on the dataset
         if (H5LTset_attribute_string( fileId_, name_.c_str(), osu.str().c_str(), it_unit->c_str() ) < 0)
		 log_fatal("Couldn't set attribute '%s' = '%s' for dataset '%s'",
                          osu.str().c_str(),it_unit->c_str(),name_.c_str());
         if (H5LTset_attribute_string( fileId_, name_.c_str(), osd.str().c_str(), it_doc->c_str() ) < 0)
		 log_fatal("Couldn't set attribute '%s' = '%s' for dataset '%s'",
                          osd.str().c_str(),it_doc->c_str(),name_.c_str());
      
   }
   
   // set the MultiRow attribute
   char ragged = description_->GetIsMultiRow();
   H5LTset_attribute_char ( fileId_, name_.c_str(), "__I3RaggedTable__", &ragged, 1);
   
   // finally, free any H5Ts we've created
   std::vector<hid_t>::iterator hid_it;
   for (hid_it = fieldHdfTypes.begin(); hid_it != fieldHdfTypes.end(); hid_it++) {
       H5Tclose(*hid_it);
   }
}

/******************************************************************************/

// Create a TableRowDescription from an existing HDF table
void I3HDFTable::CreateDescription() {
   
   // for a table already on disk, we're responsible for the description
   I3TableRowDescriptionPtr description = I3TableRowDescriptionPtr(new I3TableRowDescription());
   
   // Get the dimensions of the table
   hsize_t nfields,nrecords,i;
   H5TBget_table_info(fileId_,name_.c_str(),&nfields,&nrecords);
   
   // note how many rows this table contains
   nrows_ = nrecords;
   nrowsWithPadding_ = nrecords;
   
   // grab the table's compound datatype
   hid_t dset_id = H5Dopen(fileId_,name_.c_str());
   hid_t table_dtype = H5Dget_type(dset_id);
   H5Dclose(dset_id);
   
   // this API is not so pretty. If I get the member name by hand, the library
   // mallocs enough memory to hold the string and hands me a pointer.
   // H5TBget_field_info does _exactly_ the same thing, only that it strcpys straight
   // into the buffer I've passed, size be damned. As far as I know, there's 
   // no simple/efficient way to query the size of the attribute string beforehand.
   char**  field_names   = new char*[nfields];
   for (i = 0; i < nfields; i++) {
      field_names[i] = H5Tget_member_name( table_dtype, (unsigned)i );
   }
   
   size_t* field_sizes   = new size_t[nfields];
   size_t* field_offsets = new size_t[nfields];
   size_t type_size;
      
   H5TBget_field_info(fileId_,name_.c_str(),field_names,field_sizes,field_offsets,&type_size);
   
   description = I3TableRowDescriptionPtr(new I3TableRowDescription());
   
   hid_t hdftype;
   size_t array_size;
   std::string unit,doc;
   
   for (hsize_t i=0; i < nfields; i++) {
      hdftype = H5Tget_member_type(table_dtype,i);
      
      array_size = 1;
      I3Datatype dtype = GetI3Datatype(hdftype, &array_size);

      std::ostringstream osu,osd;
      osu << "FIELD_" << i << "_UNIT";
      osd << "FIELD_" << i << "_DOC";
      
      unit = ReadAttributeString(fileId_,name_,osu.str());
      doc  = ReadAttributeString(fileId_,name_,osd.str());

      description->AddField(std::string(field_names[i]),dtype,unit.c_str(),doc.c_str(),array_size);
      
      // release the member datatype
      H5Tclose(hdftype);
   }
   H5Tclose(table_dtype);
   
   // get the MultiRow attribute
   char ragged = 0;
   H5LTget_attribute_char ( fileId_, name_.c_str(), "__I3RaggedTable__", &ragged);
   description->isMultiRow_ = ragged;
   
   // the individual strings were malloc'd by the HDF library,
   // not new'd by me
   for (i = 0; i < nfields; i++) free(field_names[i]);
   delete[] field_names;
   delete[] field_sizes;
   delete[] field_offsets;
   
   description_ = description;
   nrows_ = nrecords;
   nrowsWithPadding_ = nrecords;
   tableCreated_ = true;
}

/******************************************************************************/

// safely read an attribute string, avoiding overflows.
// This is sort of important, as we in principle allow entire PRLs
// to be pasted into the doc string for a field. 
std::string I3HDFTable::ReadAttributeString(hid_t fileID, std::string& where, std::string attribute) {
   
   size_t attr_size;
   hsize_t dims;
   H5T_class_t type_class;
   std::string val;
   herr_t status;
   
   status = H5LTget_attribute_info(fileID,where.c_str(),attribute.c_str(),&dims,&type_class,&attr_size);
   
   if ((status < 0) || (type_class != H5T_STRING)) {
      return val;
   }
   
   char* attr_buffer = new char[attr_size];
      
   H5LTget_attribute_string(fileID,where.c_str(),attribute.c_str(),attr_buffer);
   
   val = attr_buffer;
   
   delete[] attr_buffer;
   return val;
}

/******************************************************************************/

I3HDFTable::~I3HDFTable() {}

/******************************************************************************/

std::string I3HDFTable::log_label() {
    std::string name = name_;
    if (!indexTable_) name += "(index)";
    return name;
}

void I3HDFTable::WriteRows(I3TableRowConstPtr rows) {
    writeCache_->append(*rows);
    // only write if buffer is larger than a chunk
    size_t chunks = writeCache_->GetNumberOfRows()/chunkSize_;
    // log_trace("%s Write cache contains %zu rows (%zu %zu-row chunks)",log_label().c_str(),writeCache_->GetNumberOfRows(),chunks,chunkSize_);
    if (chunks >= CHUNKTIMES) Flush(chunks*chunkSize_);
}

void I3HDFTable::Flush(size_t nrows) {
    log_trace("%s Flushing %zu rows from %zu-row cache",log_label().c_str(),nrows,writeCache_->GetNumberOfRows());
    if (nrows == 0) nrows = writeCache_->GetNumberOfRows();
    if (nrows == 0) return;
    
    const size_t rowsize = description_->GetTotalByteSize();
    const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
    const size_t* fieldSizes   = &(description_->GetFieldTypeSizes().front());
    const void* buffer = writeCache_->GetPointer();
    herr_t status = 
        H5TBappend_records(fileId_,        // file
                           name_.c_str(),  // data set name
                           nrows,	       // number of the records in buffer
                           rowsize,        // size of records in buffer
                           fieldOffsets,   // struct offsets
                           fieldSizes, 	   // size of the fields
                           buffer);		   // data to write
    if (status < 0) {
        log_fatal("failed to append rows to table.");
    } else {
        writeCache_->erase(nrows);
    }
}

I3TableRowPtr I3HDFTable::ReadRowsFromTable(size_t start, size_t nrows) const {
   if (!description_) {
      log_fatal("No I3TableRowDescription is set for this table.");
   }
   I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(description_,nrows));
   // TODO: reading every row-group individually is pretty
   // inefficient for sequential reads of from bits of compressed blocks 
   // (e.g. with TableTranscriber): come up with some sort of read-buffering scheme
   const size_t rowsize = description_->GetTotalByteSize();
   const size_t* fieldOffsets = &(description_->GetFieldByteOffsets().front());
   const size_t* fieldSizes   = &(description_->GetFieldTypeSizes().front());
   void* buffer = const_cast<void*>(rows->GetPointer());
   herr_t status = 
       H5TBread_records(fileId_,       // file
                        name_.c_str(), // data set name
                        start,         // index at which to start reading
                        nrows,         // number of records to read
                        rowsize,       // size of records in buffer
                        fieldOffsets,  // struct offsets
                        fieldSizes,    // size of the fields
                        buffer);       // where to write data
   if (status < 0) {
       log_error("(%s) error reading rows %zu--%zu",name_.c_str(),start,start+nrows);
       return I3TableRowPtr();
   } else {
       return rows;
   }
}

I3TableRowConstPtr I3HDFTable::ReadRows(size_t start, size_t nrows) const {
    if (start < readCacheExtent_.first || start+nrows > readCacheExtent_.second ) {
        size_t nrows_remaining = std::min(CHUNKTIMES*chunkSize_,nrowsWithPadding_-start);
        size_t nrows_to_read = std::max(nrows_remaining,nrows);
        log_trace("Invalidating cache and reading %zu + %zu rows",start,nrows_to_read);
        readCache_ = ReadRowsFromTable(start,nrows_to_read);
        readCacheExtent_ = std::make_pair(start,start+nrows_to_read);
    }
    size_t cacheStart = start - readCacheExtent_.first;
    return I3TableRowPtr(new I3TableRow(*readCache_,cacheStart,cacheStart+nrows));
}

std::pair<size_t,size_t> I3HDFTable::GetRangeForEvent(size_t index) const {
    if (indexTable_) {
        if (index > indexTable_->GetNumberOfRows()-1) {
            return std::pair<unsigned int,unsigned int>(0,0);
        }
        boost::shared_ptr<I3HDFTable> indexTable = boost::static_pointer_cast<I3HDFTable>(indexTable_);
        I3TableRowPtr indexrow = boost::const_pointer_cast<I3TableRow>(indexTable->ReadRows(index,1));
        size_t start = indexrow->Get<tableio_size_t>("start");
        size_t end = indexrow->Get<tableio_size_t>("stop");
        return std::pair<size_t,size_t>(start,end);
    } else {
        log_fatal("(%s) This table has no index, and is thus write-only.",name_.c_str());
        return std::pair<size_t,size_t>();
    }
}

