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

#include <tableio/utils/I3TableTranscriber.h>
#include <tableio/I3TableRowDescription.h>
#include <tableio/I3TableRow.h>

#include <dataclasses/physics/I3EventHeader.h>

#include <boost/make_shared.hpp>

I3TableTranscriber::I3TableTranscriber(I3TableServicePtr input, I3TableServicePtr output) 
    : inputService_(input), outputService_(output), nEvents_(0) {
        
        std::vector<std::string>::iterator it;
        std::vector<std::string> inputTables = inputService_->GetTableNames();
        std::map<std::string, I3TablePtr> inputTableMap;
        I3TablePtr table;
        size_t nEvents = 0;
        std::string nEvents_reference;
        for (it = inputTables.begin(); it != inputTables.end(); it++) {
            table = inputService_->GetTable(*it, I3TableRowDescriptionConstPtr());
            
            if (nEvents == 0) {
                nEvents = table->GetNumberOfEvents();
                nEvents_reference = table->GetName();
            } else if (table->GetNumberOfEvents() != nEvents) {
                log_fatal("Input tables '%s' '%s' have different sizes (%zu vs. %zu)",
                    nEvents_reference.c_str(),table->GetName().c_str(),nEvents,table->GetNumberOfEvents());
            }

            if ((nEvents > 0) && (!indexer_))
                indexer_ = boost::make_shared<I3IndexColumnsGenerator>(table->GetDescription());

           
            inputTableMap[*it] = table;
        }
        
        if (nEvents == 0) {
            log_warn("Input file contains no events, skipping file...");
            return;
        }

        outputService_->SetIndexConverter(indexer_);

        nEvents_ = nEvents;
        
        std::map<std::string, I3TablePtr>::iterator t_it;
        I3TablePtr otable;
        std::string name;
        for (t_it = inputTableMap.begin(); t_it != inputTableMap.end(); t_it++) {
            name = t_it->first;
            table = t_it->second;
            log_trace("Creating output table '%s'",name.c_str());
            otable = ConnectTable(name,*table->GetDescription());
            transcriptions_.push_back(std::make_pair(table,otable));
        }
}

void I3TableTranscriber::Execute() {
    Execute(nEvents_);
}

void I3TableTranscriber::Execute(size_t nframes) {
    // don't overrun the table
    if (nframes > nEvents_) nframes = nEvents_;
    size_t i;
    std::vector<std::pair<I3TablePtr,I3TablePtr> >::iterator pair_it;
    I3TablePtr in,out;
    I3EventHeaderPtr header;
    I3TableRowPtr rows;

    // loop over the given number of row-groups
    for (i = 0; i<nframes; i++) {
        log_trace("Transcribing event #%zu",i+1);
        for (pair_it = transcriptions_.begin(); pair_it != transcriptions_.end(); pair_it++) {
            in = pair_it->first;
            out = pair_it->second;
            log_trace("Getting rows for '%s'",in->GetName().c_str());
            rows = boost::const_pointer_cast<I3TableRow>(in->GetRowForEvent(i));
            // don't transcribe padding rows
            // convention: if a block is padding, its first row
            // should always be marked as such
            if ((!rows) || (!rows->Get<bool>("exists"))) continue;
            header = indexer_->Resurrect(rows);

            log_trace("R%u/E%u '%s': %zu row(s)",
                header->GetRunID(),header->GetEventID(),in->GetName().c_str(),rows->GetNumberOfRows());
            
            out->AddRow(header,rows);
        }
    }
    
    Finish();
}

void I3TableTranscriber::Finish() {
    // disconnect from all tables
    std::vector<std::pair<I3TablePtr,I3TablePtr> >::iterator pair_it;
    for (pair_it = transcriptions_.begin(); pair_it != transcriptions_.end(); pair_it++) {
        DisconnectTable(pair_it->second);
    }

    /* NB: the user is responsible for closing the output service. */
}

/******************************************************************************/

I3TablePtr I3TableTranscriber::ConnectTable(std::string tableName, 
                                       const I3TableRowDescription& description) {
    log_debug("Connecting to table %s", tableName.c_str());
    I3TableRowDescriptionPtr descPointer = 
        I3TableRowDescriptionPtr(new I3TableRowDescription(description));
    I3TablePtr table = outputService_->GetTable(tableName, descPointer);
    if (!table) {
        log_fatal("couldn't get table %s from the service", tableName.c_str());
    }
    table->SetConnectedToWriter(true);
    return table;
}

/******************************************************************************/

void I3TableTranscriber::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str());
    table->Align();
    table->SetConnectedToWriter(false);
    table.reset();
}

/******************************************************************************/


