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

#include "tableio/I3Table.h"
#include "tableio/I3TableRow.h"
#include "tableio/I3TableRowDescription.h"
#include "tableio/I3TableService.h"
#include "tableio/I3Converter.h"

/******************************************************************************/

I3Table::I3Table(I3TableService& service, 
                 std::string name,
                 I3TableRowDescriptionConstPtr description) :
    service_(service),
    name_(name),
    description_(description),
    indexTable_(),
    nevents_(0),
    nrows_(0),
    nrowsWithPadding_(0),
    connected_(false),
    tableCreated_(false) {
    // implementation pending
}

/******************************************************************************/

I3Table::~I3Table() { Flush(); }; // Flush?

// Default flush implementation does nothing
void I3Table::Flush(const size_t nrows) {};

/******************************************************************************/

bool I3Table::IsConnectedToWriter() {
    return connected_;
}

/******************************************************************************/

void I3Table::SetConnectedToWriter(bool connected) {
    connected_ = connected;
}

/******************************************************************************/

I3Table::AlignmentType I3Table::GetAlignmentType() {
    return MultiRow;
}

/******************************************************************************/
        
I3TableRowDescriptionConstPtr I3Table::GetDescription() {
    return description_;
}

/******************************************************************************/

bool I3Table::DoPadding(){
  // only pad the data table itself if this is a non-ragged dataset
  // or if the table type is aligned by construction
  bool do_padding =
    description_->GetUsePadding()&&
    ((GetAlignmentType() == Strict) ||                      
     ((GetAlignmentType() == MultiRow) && (!description_->GetIsMultiRow())));    
  return do_padding;
}

/******************************************************************************/

void I3Table::AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row) {  
    // sanity check: padding behavior is different for ragged tables
    size_t nrows = row->GetNumberOfRows(); 
    if ((nrows != 1) && (!description_->GetIsMultiRow())) {
        log_fatal("(%s) Converter reported %zu rows for a single-row object! Multi-row objects must be marked by their converters.",name_.c_str(),nrows);
    }
    
    I3TableRowConstPtr padding;
    if (DoPadding() && header) {
        padding = service_.GetPaddingRows(lastHeader_, header, description_);
        if (padding) {
            log_trace("(%s) Writing %zu padding rows",name_.c_str(),padding->GetNumberOfRows());
	    for (size_t r = 0; r < padding->GetNumberOfRows(); ++r) {
	      WriteRows(padding->GetSingleRow(r));
	    }
            nrowsWithPadding_ += padding->GetNumberOfRows();
        }
    }    
    // always pad the index table if it exists, since the index is alway a single row
    if (indexTable_) {
        assert(header);  
        padding = service_.GetPaddingRows(lastHeader_, header, service_.GetIndexDescription());
        if (padding) indexTable_->WriteRows(padding);
    }

    WriteRows(row);
    
    if (indexTable_) {
      assert(header);
      I3TableRowPtr index_row = I3TableRowPtr(new I3TableRow(service_.GetIndexDescription(),1));
      index_row->Set<uint32_t>("Run",header->GetRunID());
      index_row->Set<uint32_t>("Event",header->GetEventID());
      index_row->Set<bool>("exists",true);
      index_row->Set<tableio_size_t>("start",static_cast<tableio_size_t>(nrowsWithPadding_));
      index_row->Set<tableio_size_t>("stop",static_cast<tableio_size_t>(nrowsWithPadding_+row->GetNumberOfRows()));
      log_trace("(%s) Writing row to index table. start: %zu end: %zu",
                name_.c_str(),nrowsWithPadding_,nrowsWithPadding_+row->GetNumberOfRows());
      
      indexTable_->WriteRows(index_row);
    }
    
    nevents_++;
    nrows_ += row->GetNumberOfRows();
    nrowsWithPadding_ += row->GetNumberOfRows();
    log_debug("(%s) nevents = %zu, nrows = %zu, nrowsWithPadding = %zu", name_.c_str(),
              nevents_, nrows_, nrowsWithPadding_);

    if (header){
      lastHeader_ = I3EventHeaderConstPtr(new I3EventHeader(*header));
      service_.HeaderWritten(lastHeader_,row->GetNumberOfRows());
    }
}

/******************************************************************************/

// force the table to write out padding rows
// NB: this is triggered by passing a NULL header pointer
void I3Table::Align() {
    log_debug("(%s) Entering the Align function......", name_.c_str());
    I3TableRowConstPtr padding;
    log_debug("Alignmnet type = %d", GetAlignmentType());
    log_debug("Is there a last_header_? %d", (lastHeader_!=NULL));
    // Note: This function gets called on the "Master Tree" at the very end, which has no header.
    // But we *do* want to do this padding thing on that Master Tree, because it has no rows yet.
    if (DoPadding()) {
        padding = service_.GetPaddingRows(lastHeader_, I3EventHeaderConstPtr(), description_);
        if (padding) {
            log_trace("(%s) Finalizing alignment with %zu padding rows",name_.c_str(),padding->GetNumberOfRows());
            for (size_t r = 0; r < padding->GetNumberOfRows(); ++r) {
              WriteRows(padding->GetSingleRow(r));
            }
            nrowsWithPadding_ += padding->GetNumberOfRows();
        }
    }
    
    // always pad the index table if it exists, since the index is alway a single row
    if (indexTable_) {
        padding = service_.GetPaddingRows(lastHeader_, I3EventHeaderConstPtr(), service_.GetIndexDescription());
        if (padding) indexTable_->WriteRows(padding);
        indexTable_->Flush();
    }

    lastHeader_ = service_.GetLastHeader();
    Flush();
}

/******************************************************************************/

I3TableRowConstPtr I3Table::GetRowForEvent(size_t index) const {
    std::pair<size_t,size_t> range;
    
    range = GetRangeForEvent(index);
    if (range.second == 0) {
        return I3TableRowPtr();
    } else {
        return ReadRows(range.first,range.second-range.first);
    }
}

/******************************************************************************/

std::pair<size_t,size_t> I3Table::GetRangeForEvent(size_t index) const {
    log_fatal("This table is write-only.");
    return std::pair<size_t,size_t>();
}


/******************************************************************************/

// default implementation is write-only
I3TableRowConstPtr I3Table::ReadRows(size_t start, size_t nrows) const {
	log_fatal("This table is write-only.");
	return I3TableRowConstPtr();
}


/******************************************************************************/

size_t I3Table::GetNumberOfEvents() const {
   return nevents_; 
}

/******************************************************************************/

size_t I3Table::GetNumberOfRows() const {
    return nrows_;
}

/******************************************************************************/

std::string I3Table::GetName() const {
    return name_;
}

/******************************************************************************/
        
I3TableRowPtr I3Table::CreateRow(size_t nrows) {
    return I3TableRowPtr( new I3TableRow(description_, nrows) );
}

/******************************************************************************/
