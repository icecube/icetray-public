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

#include "tableio/I3TableService.h"
#include "tableio/I3TableRow.h"
#include "tableio/converter/I3IndexColumnsGenerator.h"

/******************************************************************************/

I3TableService::I3TableService()  { 
    // Set up a semi-sensible default
    SetIndexConverter(I3ConverterPtr(new I3IndexColumnsGenerator));
}

/******************************************************************************/

std::vector<std::string> I3TableService::GetTableNames() {
	std::vector<std::string> out;
	out.reserve(tables_.size());
	std::map<std::string, I3TablePtr>::const_iterator it;
	
	for(it = tables_.begin(); it != tables_.end(); it++) out.push_back(it->first);
	return out;
}

/******************************************************************************/
        
I3TablePtr I3TableService::GetTable(std::string name,
                                    I3TableRowDescriptionConstPtr description) {
    std::map<std::string, I3TablePtr>::iterator it;
    I3TablePtr table;
    it = tables_.find(name);

    if (it != tables_.end()) {
       table = it->second;
    } else if (description) {
       // create table if description is not NULL
      table = CreateTable(name,description);
       tables_[name] = table;
    }

    // otherwise, return NULL
    return table;
}

/******************************************************************************/
        
/*
I3TablePtr I3TableService::CreateTable(std::string tableName,
                                       I3TableRowDescriptionConstPtr description) {
    // implementation pending
    log_fatal("not implemented");
    return I3TablePtr();
}
*/

/******************************************************************************/
        
bool I3TableService::EventHeadersEqual(const I3EventHeader& header1,
                                       const I3EventHeader& header2) {
    return ( (header1.GetRunID() == header2.GetRunID()) &&
             (header1.GetEventID() == header2.GetEventID()) &&
             (header1.GetSubEventID() == header2.GetSubEventID()) &&
             (header1.GetSubEventStream() == header2.GetSubEventStream()));
}

/******************************************************************************/

// A table phones home to report that it has written a new event
void I3TableService::HeaderWritten(I3EventHeaderConstPtr lastHeader, size_t nrows) {
   if (lastHeader == NULL) return;
   else {
      if (eventHeaderCache_.size() == 0) {
         log_trace("First event: Run %d Event %d",lastHeader->GetRunID(),lastHeader->GetEventID());
         eventHeaderCache_.push_back(lastHeader);
      } else if ( !EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) && (nrows>0)) {
         log_trace("New event: Run %d Event %d",lastHeader->GetRunID(),lastHeader->GetEventID());
         eventHeaderCache_.push_back(lastHeader);
      }
   }
}

/******************************************************************************/

// Get the last header the service has seen
I3EventHeaderConstPtr I3TableService::GetLastHeader() {
   if (!eventHeaderCache_.size()) return I3EventHeaderConstPtr();
   return eventHeaderCache_.back();
}

/******************************************************************************/

// Get the last header the service has seen
I3TableRowDescriptionConstPtr I3TableService::GetIndexDescription() {
   return indexDescription_;
}

/******************************************************************************/

void I3TableService::SetIndexConverter(I3ConverterPtr gen) {
    ticConverter_ = gen;
    // My house, my rules.
    I3TableRowDescriptionPtr desc =
        boost::const_pointer_cast<I3TableRowDescription>(ticConverter_->GetDescription(I3EventHeader()));
    desc->AddField<tableio_size_t>("start", "", "Offset at which the rows for this event start");
    desc->AddField<tableio_size_t>("stop", "", "Offset at which the rows for the next event start");
    indexDescription_ = desc;
}

/******************************************************************************/
        
I3TableRowConstPtr I3TableService::GetPaddingRows(I3EventHeaderConstPtr lastHeader,
                                                  I3EventHeaderConstPtr newHeader,
                                             I3TableRowDescriptionConstPtr description){
   // catch the cases where padding is not necessary 
   if (eventHeaderCache_.size() == 0) { // first call, first event 
     log_trace("Event header cache is empty, no padding required");
     return I3TableRowConstPtr();
   }

   size_t nrows = 0;
   std::vector<I3EventHeaderConstPtr>::reverse_iterator rit;

   if (!lastHeader) { // this table is reporting for the first time
      if (!newHeader) {
          log_trace("This table is reporting for the first time with a NULL header, padding out the table completely.");
          rit = eventHeaderCache_.rend()-1;
          nrows = eventHeaderCache_.size();
      } else if (EventHeadersEqual(*newHeader, *(eventHeaderCache_.back()))) {
         log_trace("This table is reporting for the first time; R%u E%u is the current event",newHeader->GetRunID(),newHeader->GetEventID());
         rit = eventHeaderCache_.rend()-1;
         nrows = eventHeaderCache_.size()-1;
      } else {
         log_trace("This table is reporting for the first time; R%u E%u is a previous event",newHeader->GetRunID(),newHeader->GetEventID());
         rit = eventHeaderCache_.rend()-1;
         nrows = eventHeaderCache_.size();
      }
   } else { // this table has written something in the past, so we search the cache for its last header
   
      // in sync, lastHeader is the same as the last entry in the cache
      if ( EventHeadersEqual(*lastHeader, *(eventHeaderCache_.back())) ) { 
           log_trace("Table in sync with the cache, no padding required");
           return I3TableRowConstPtr();
      }
   
      // padding is necessary -> figure out how many rows
      // 2 scenarios: l: lastHeader, h: newHeader, 1...6 cached headers
      // A: 1,2,3,l,4,5,6,h -> 3 padding rows
      // B: 1,2,3,l,4,5,(6=h) -> 2 padding rows
      
      // scenario A (OR: newHeader NULL => table is finished, pad out all missing rows)
      if (!newHeader || !EventHeadersEqual(*newHeader, *(eventHeaderCache_.back()))) {
         log_trace("Padding up to last known event header");   
         for(rit = eventHeaderCache_.rbegin(); rit != eventHeaderCache_.rend() && !EventHeadersEqual(**rit, *lastHeader); rit++) nrows++;
         --rit; // go to the first missed event (reverse iterator)
                // fill up to and including this event
      } else { // scenario B: another table already reported this event
         log_trace("Another table already reported this event.");
         // start at 
         for(rit = eventHeaderCache_.rbegin(); rit != eventHeaderCache_.rend() && !EventHeadersEqual(**rit, *lastHeader); rit++) {
            nrows++;
         }
         --rit;  // go to the first missed event (reverse iterator)
         nrows -= 1; // fill up to but not including this event
      }

   }
      
   log_trace("nrows = %zu",nrows);
   if (nrows == 0) return I3TableRowPtr();
   I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(description, nrows));
   // NB: special case for fieldless master table needed by ROOTWriter
   if (description->GetNumberOfFields() > 0) {
       I3FramePtr frame; // assume ticConv doesn't need the frame
       for (size_t i=0; i< nrows; ++i) {
          rows->SetCurrentRow(i);
          ticConverter_->Convert(*rit, rows,frame);
          rit--;
       }
   }
   return rows;
}

/******************************************************************************/

void I3TableService::Finish() {
    bool finished = true;
    std::map<std::string, I3TablePtr>::iterator table_it = tables_.begin();
    
    for ( ; table_it != tables_.end(); ++table_it) {
        if (table_it->second->IsConnectedToWriter()) {
            finished = false;
            break;
        } else {
            table_it->second->Flush();
        }
        log_debug("In Finish, about to do a final align on %s", table_it->second->GetName().c_str());
        table_it->second->Align();
    }

    /* Only close the file if all the tables are disconnected. */
    if (finished) CloseFile();
}

I3TableService::~I3TableService() {    
    std::map<std::string, I3TablePtr>::iterator table_it = tables_.begin();
    
    for ( ; table_it != tables_.end(); ++table_it) {
        if (table_it->second->IsConnectedToWriter()) {
            log_error("Table '%s' is still connected, which means that the "
                "output file was never properly closed. This is a BUG!\n",
                table_it->first.c_str());
	    log_error("This can happen when FilterMask changes defintions in the middle of an I3Tray run.");
	    log_error("Check to make sure different years of processing haven't been merged together.");
            break;
        } 
    }
} 
