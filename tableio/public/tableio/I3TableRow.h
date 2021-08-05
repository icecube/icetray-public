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

#ifndef	I3_TABLEROW_H_INCLUDED
#define I3_TABLEROW_H_INCLUDED

// icetray includes
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "tableio/I3TableRowDescription.h"
#include "tableio/I3MemoryChunk.h"

I3_FORWARD_DECLARATION(I3TableRow);

class I3TableRow {
    public:
        I3TableRow(I3TableRowDescriptionConstPtr description, size_t nrows=1);
        I3TableRow(const I3TableRow& rhs);
        I3TableRow(const I3TableRow& rhs, size_t start, size_t stop);
        I3TableRow& operator=(const I3TableRow& rhs);

        ~I3TableRow();
        
        // set the current row one which following Set and Get calls operate
        void SetCurrentRow(size_t row);
        size_t GetCurrentRow() const;

        I3TableRowPtr GetSingleRow(size_t row) const;
        
        // set the value of a field
        template<class T>
        void Set(const std::string& fieldName, T value, bool all = false);
        
        // set the value of a field by index
        template<class T>
        void Set(size_t index, T value, bool all = false);
        
        // get the value of a field in the current row
        template<class T>
        T Get(const std::string& fieldName);
        
        // get the value of a field in the current row by index
        template<class T>
        T Get(size_t index);
        
        // get a pointer to the beginning of field for the current row
        template<class T>
        T* GetPointer(const std::string& fieldName);
        
        // get a pointer to the beginning of field for the current row
        template<class T>
        T* GetPointer(size_t index);
        
        // get a pointer to the beginning of field for the given row
        template<class T>
        T* GetPointer(const std::string& fieldName, size_t row);
        
        // get a pointer to the beginning of field for the given row
        template<class T>
        T* GetPointer(size_t index, size_t row);

        // get a pointer to the beginning of field for the current row
        template<class T>
        const T* GetPointer(const std::string& fieldName) const;
        
        // get a pointer to the beginning of field for the current row
        template<class T>
        const T* GetPointer(size_t index) const;
        
        // get a pointer to the beginning of field for the given row
        template<class T>
        const T* GetPointer(const std::string& fieldName, size_t row) const;
        
        // get a pointer to the beginning of field for the given row
        template<class T>
        const T* GetPointer(size_t index, size_t row) const;

        // get a void pointer to whole memory block
        void const* GetPointer() const;
        
        // get a void pointer to a particular row
        void const* GetPointerToField(size_t index, size_t row) const;
        
        // get a void pointer to the beginning of a particular row
        void const* GetPointerToRow(size_t row) const;
        
        // and a non-const version of same
        void* GetPointerToField(size_t index, size_t row);
        
        
        I3TableRowDescriptionConstPtr GetDescription() const;

        size_t GetNumberOfRows() const;
        void SetNumberOfRows(size_t nrows);

        void SetEnumsAreInts( bool flag ) { enums_are_ints_ = flag; };
        
        void reserve(size_t nrows);
        void erase(size_t nrows);
        void append(const I3TableRow& rhs);

    private:
        // set field with name fieldName to value for all rows
        template<class T>
        void SetAll(size_t index, T value);
        
        // set the value of a field in the current row
        template<class T>
        void SetCurrent(size_t index, T value);
        
        // check if the templated type is compatible with the
        // field at index
        template<class T>
        bool CheckType(size_t index) const;
        
        I3TableRow();
        void init();
        void expand(size_t nrows);

        I3TableRowDescriptionConstPtr description_;
        size_t nrows_;
        size_t capacity_;
        size_t currentRow_;
        I3MemoryChunk* data_;
        bool enums_are_ints_;

};

I3_POINTER_TYPEDEFS( I3TableRow );

/******************************************************************************/

template<class T>
bool I3TableRow::CheckType(size_t index) const {
    bool compatible = false;
    I3Datatype requested_dtype = I3DatatypeFromNativeType<T>();
    I3Datatype this_dtype = description_->GetFieldTypes().at(index);
    compatible = this_dtype.CompatibleWith(requested_dtype,enums_are_ints_);
    if (!compatible) {
        log_fatal("The requested type %s is not compatible with %s, the type of field '%s'.",
                    requested_dtype.AsString().c_str(),this_dtype.AsString().c_str(),
                    description_->GetFieldNames().at(index).c_str());
    }
    return compatible;
}

template<class T>
void I3TableRow::Set(const std::string& fieldName, T value, bool all) {
    size_t index = description_->GetFieldColumn(fieldName);
    //log_trace("I3TableRow::Set:  field %s has index %zu", fieldName.c_str(), index);
    if (index >= description_->GetNumberOfFields()) {
        log_fatal("Tried to set unknown column '%s'",fieldName.c_str());
    }
    if (all) {
        SetAll(index,value);
    } else { 
        SetCurrent(index,value);
    }
}

template<class T>
void I3TableRow::Set(size_t index, T value, bool all) {
    if (index >= description_->GetNumberOfFields()) {
        log_fatal("Tried to set column which is not in [0,%zu]",description_->GetNumberOfFields());
    }
    if (all) {
        SetAll(index,value);
    } else {
        SetCurrent(index,value);
    }
}


template<class T>
void I3TableRow::SetCurrent(size_t index, T value) {
    T*  pointer = GetPointer<T>(index, currentRow_); 
    *pointer = value;
}

template<class T>
void I3TableRow::SetAll(size_t index, T value) {
    for (size_t row = 0; row < nrows_; ++row) {
       T*  pointer = GetPointer<T>(index, row); 
       *pointer = value;
    }
}

/******************************************************************************/
        
template<class T>
T I3TableRow::Get(const std::string& fieldName) {
    return *GetPointer<T>(fieldName, currentRow_);
}

template<class T>
T I3TableRow::Get(size_t index) {
    if (index >= description_->GetNumberOfFields())
        log_fatal("Tried to get column which is not in [0,%zu]",description_->GetNumberOfFields());
    return *GetPointer<T>(index, currentRow_);
}


/******************************************************************************/

template<class T>
T* I3TableRow::GetPointer(const std::string& fieldName) {
    return GetPointer<T>(fieldName, currentRow_);
}

template<class T>
T* I3TableRow::GetPointer(size_t index) {
    return GetPointer<T>(index, currentRow_);
}


template<class T>
T* I3TableRow::GetPointer(const std::string& fieldName, size_t row) {
    size_t index;
    if ( (index = description_->GetFieldColumn(fieldName)) >= description_->GetNumberOfFields() ) 
        log_fatal("trying to get the address of unknown field %s", fieldName.c_str());

    return GetPointer<T>(index, row);
}

template<class T>
T* I3TableRow::GetPointer(size_t index, size_t row) {
    
    CheckType<T>(index);

    if ( !(row < nrows_) )
        log_fatal("requested pointer to row %zu which is not in [0,%zu]", row, nrows_);

    return reinterpret_cast<T*>(&(reinterpret_cast<uint8_t*>(
        data_)[I3MEMORYCHUNK_SIZE*description_->GetTotalChunkSize()*row + 
        description_->GetFieldByteOffsets().at(index)]));
}


// const versions:
template<class T>
const T* I3TableRow::GetPointer(const std::string& fieldName) const {
    return GetPointer<T>(fieldName, currentRow_);
}

template<class T>
const T* I3TableRow::GetPointer(size_t index) const {
    return GetPointer<T>(index, currentRow_);
}


template<class T>
const T* I3TableRow::GetPointer(const std::string& fieldName, size_t row) const {
    size_t index;
    if ( (index = description_->GetFieldColumn(fieldName)) >= description_->GetNumberOfFields() ) 
        log_fatal("trying to get the address of unknown field %s", fieldName.c_str());

    return GetPointer<T>(index, row);
}

template<class T>
const T* I3TableRow::GetPointer(size_t index, size_t row) const {
    
    CheckType<const T>(index);

    if ( !(row < nrows_) )
        log_fatal("requested pointer to row %zu which is not in [0,%zu]", row, nrows_);
    
    return reinterpret_cast<const T*>(&(reinterpret_cast<const uint8_t*>(
        data_)[I3MEMORYCHUNK_SIZE*description_->GetTotalChunkSize()*row + 
        description_->GetFieldByteOffsets().at(index)]));

}


#endif
