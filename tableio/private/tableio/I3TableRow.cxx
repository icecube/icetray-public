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

#include <cassert>

#include "tableio/I3TableRow.h"

/******************************************************************************/

void I3TableRow::init() {
    // assume nrows_ and description_ are set; data_ == 0
    const size_t totalChunkSize = capacity_ * description_->GetTotalChunkSize();
    data_ = new I3MemoryChunk[totalChunkSize];

    // initialize memory block with zeros
    memset(data_, 0, I3MEMORYCHUNK_SIZE*totalChunkSize);
}

/******************************************************************************/

I3TableRow::I3TableRow(I3TableRowDescriptionConstPtr description, 
                       size_t nrows) :
    description_(description),
    nrows_(nrows),
    capacity_(nrows),
    currentRow_(0),
    enums_are_ints_(false)
{
        init();
}



/******************************************************************************/

I3TableRow::~I3TableRow() {
    delete[] data_;
}

/******************************************************************************/

void I3TableRow::expand(size_t nrows) {
    log_trace("Expanding to %zu rows",nrows);
    if (nrows <= nrows_) {
        log_fatal("Attempted to change size of I3TableRow from %zu to %zu!",nrows_,nrows);
    }
    const size_t totalChunkSize = nrows * description_->GetTotalChunkSize();
    const size_t totalByteSize = nrows * description_->GetTotalByteSize();
    const size_t currentByteSize = nrows_ * description_->GetTotalByteSize();
    I3MemoryChunk* newdata = new I3MemoryChunk[totalChunkSize];
    // copy over old buffer
    memcpy(newdata, data_, currentByteSize);
    // zero the remaining memory
    memset( &newdata[nrows_ * description_->GetTotalChunkSize()], 0, totalByteSize-currentByteSize);
    
    // delete the old data array and replace it with the new one
    delete[] data_;
    data_ = newdata;
    // update the capacity
    capacity_ = nrows;
}

/******************************************************************************/

void I3TableRow::reserve(size_t nrows) {
    if (nrows > capacity_) expand(nrows);
}

/******************************************************************************/

// erase the first nrows
void I3TableRow::erase(size_t nrows) {
    if (nrows == 0) return; // no-op
    if (nrows > nrows_) nrows = nrows_;
    const size_t doomed_block_size = nrows * description_->GetTotalByteSize();
    const size_t remaining_block_size = (nrows_-nrows) * description_->GetTotalByteSize();
    // shift the remaining bytes down to the front of the array
    memmove( data_, &data_[nrows * description_->GetTotalChunkSize()], remaining_block_size );
    // zero the former location of the remaining bytes
    memset( &data_[(nrows_-nrows) * description_->GetTotalChunkSize()], 0, doomed_block_size );
    // decrement the container size
    nrows_ -= nrows;
}

/******************************************************************************/

void I3TableRow::append(const I3TableRow& rhs) {
    const size_t required_rows = nrows_ + rhs.GetNumberOfRows();
    const size_t bytes_to_write = rhs.GetNumberOfRows()*description_->GetTotalByteSize();
    const size_t index = nrows_ * description_->GetTotalChunkSize();
    if (required_rows > capacity_) {
        expand(std::max(required_rows,2*capacity_));
    }
    memcpy( &data_[index], rhs.GetPointer(), bytes_to_write);
    
    nrows_ = required_rows;
}

/******************************************************************************/

I3TableRow::I3TableRow(const I3TableRow& rhs) {
    description_ = rhs.GetDescription();
    nrows_ = rhs.GetNumberOfRows();
    capacity_ = nrows_;
    currentRow_ = 0;
    enums_are_ints_ = false;
    
    size_t totalChunkSize = nrows_*rhs.GetDescription()->GetTotalChunkSize();
    size_t totalByteSize = nrows_*rhs.GetDescription()->GetTotalByteSize();
    data_ = new I3MemoryChunk[totalChunkSize];
    memcpy( data_, rhs.GetPointer(), totalByteSize );
}

/******************************************************************************/

I3TableRow::I3TableRow(const I3TableRow& rhs, size_t start, size_t stop) {
    assert( stop <= rhs.GetNumberOfRows() );
    assert( stop > start );
    description_ = rhs.GetDescription();
    nrows_ = stop - start;
    capacity_ = nrows_;
    currentRow_ = 0;
    enums_are_ints_ = false;
    
    size_t totalChunkSize = nrows_*rhs.GetDescription()->GetTotalChunkSize();
    size_t totalByteSize = nrows_*rhs.GetDescription()->GetTotalByteSize();
    data_ = new I3MemoryChunk[totalChunkSize];
    memcpy( data_, rhs.GetPointerToRow(start), totalByteSize );
}


/******************************************************************************/

I3TableRow& I3TableRow::operator=(const I3TableRow& rhs) {
    size_t totalByteSize = rhs.GetNumberOfRows()*rhs.GetDescription()->GetTotalByteSize();
    size_t totalChunkSize = rhs.GetNumberOfRows()*rhs.GetDescription()->GetTotalByteSize();
    if (totalByteSize == nrows_*description_->GetTotalByteSize()) {
        memcpy( data_, rhs.GetPointer(), totalByteSize );
    }
    else {
        delete[] data_;
        data_ = new I3MemoryChunk[totalChunkSize];
        memcpy( data_, rhs.GetPointer(), totalByteSize );
    }
    description_ = rhs.GetDescription();
    nrows_ = rhs.GetNumberOfRows();
    capacity_ = nrows_;
    currentRow_ = 0;
    return *this;
}


/******************************************************************************/

void const* I3TableRow::GetPointer() const {
    return static_cast<void*>(data_);
}

void const* I3TableRow::GetPointerToRow(size_t row) const {
    return static_cast<void*>( &data_[description_->GetTotalChunkSize()*row] );
}


void const* I3TableRow::GetPointerToField(size_t index, size_t row) const {
    if (row >= nrows_) log_fatal("Tried to get pointer to a row not in range [0,%zu)", nrows_);
    return reinterpret_cast<void const*>(&(reinterpret_cast<uint8_t*>(
        data_)[I3MEMORYCHUNK_SIZE*description_->GetTotalChunkSize()*row + 
        description_->GetFieldByteOffsets().at(index)]));
}

void* I3TableRow::GetPointerToField(size_t index, size_t row) {
    if (row >= nrows_) log_fatal("Tried to get pointer to a row not in range [0,%zu)", nrows_);
    return reinterpret_cast<void*>(&(reinterpret_cast<uint8_t*>(
        data_)[I3MEMORYCHUNK_SIZE*description_->GetTotalChunkSize()*row + 
        description_->GetFieldByteOffsets().at(index)]));
}

/******************************************************************************/

I3TableRowDescriptionConstPtr I3TableRow::GetDescription() const {
    return description_;
}

/******************************************************************************/

size_t I3TableRow::GetNumberOfRows() const {
    return nrows_;
}

/******************************************************************************/

void I3TableRow::SetNumberOfRows(size_t nrows) {
    i3_assert(description_);
    
    if (nrows_ == nrows)
        return;

    nrows_ = nrows;
    currentRow_ = 0;
    delete[] data_;
    init();
}

/******************************************************************************/

void I3TableRow::SetCurrentRow(size_t row) {
    if (nrows_ <= row)
        log_fatal("try to set current row to %zu outside of [0,%zu)", row, nrows_);
    currentRow_ = row;
}

/******************************************************************************/

size_t I3TableRow::GetCurrentRow() const {
    return currentRow_;
}

/******************************************************************************/

I3TableRowPtr I3TableRow::GetSingleRow(size_t row) const {
    I3TableRowPtr result(new I3TableRow(description_, 1));
    memcpy(result->data_, &data_[row*description_->GetTotalChunkSize()], description_->GetTotalByteSize());
    return result;
}

/******************************************************************************/

// Explictly cast booleans to true/false
template<>
void I3TableRow::Set<bool>(const std::string& fieldName, bool value, bool all) {
    size_t index = description_->GetFieldColumn(fieldName);
    if (index >= description_->GetNumberOfFields())
        log_fatal("Tried to set unknown field '%s'",fieldName.c_str());
   if (all) SetAll(index,(value!=false));
   else SetCurrent(index,(value!=false));
}

/******************************************************************************/

// For the times when you really want to blow your leg off...
template<>
void* I3TableRow::GetPointer(const std::string& fieldName, size_t row) {
    size_t index;
    if ( (index = description_->GetFieldColumn(fieldName)) >= description_->GetNumberOfFields() ) 
        log_fatal("trying to get the address of unknown field %s", fieldName.c_str());

    if ( !(row < nrows_) )
        log_fatal("requested pointer to row %zu which is not in [0,%zu]", row, nrows_);
    
    return reinterpret_cast<void*>(&(reinterpret_cast<uint8_t*>(
        data_)[I3MEMORYCHUNK_SIZE*description_->GetTotalChunkSize()*row + 
        description_->GetFieldByteOffsets().at(index)]));
}

