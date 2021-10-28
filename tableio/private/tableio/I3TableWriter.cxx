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

#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3RecoPulse.h>

#include "tableio/I3TableWriter.h"
#include "tableio/I3TableService.h"
#include "tableio/converter/I3IndexColumnsGenerator.h"
#include <I3/name_of.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

// Some things can pretend to be I3RecoPulseSeriesMaps, and we want them
// to do that, so wrap frame->Get(). Any similar future hacks should also
// go here.
inline I3FrameObjectConstPtr GetFrameObject(I3FramePtr frame, std::string name)
{
    I3FrameObjectConstPtr object = frame->Get<I3RecoPulseSeriesMapConstPtr>(name);
    if (!object)
        object = frame->Get<I3FrameObjectConstPtr>(name);

    return object;
}

/******************************************************************************/

I3TableWriter::I3TableWriter(I3TableServicePtr service, std::vector<I3ConverterMillPtr>& converters,
    std::vector<std::string>& streams) : streams_(streams) {
    service_ = service;
    boost::shared_ptr<I3IndexColumnsGenerator> indexer =
        boost::make_shared<I3IndexColumnsGenerator>(streams_);
    ticConverter_ = indexer;
    service_->SetIndexConverter(boost::make_shared<I3IndexColumnsGenerator>(*indexer));
    
    // pull in the converters registered in Python-land instead
    std::vector<I3ConverterPtr>::const_iterator it_conv __attribute__((unused));
    std::copy(converters.begin(),converters.end(),std::back_inserter(converterCache_));    
}

/******************************************************************************/

I3TableWriter::~I3TableWriter() {};

/******************************************************************************/

template <typename T>
std::string name_of(const boost::shared_ptr<T> obj) {
    return I3::name_of(typeid(*(obj.get())));
}

/******************************************************************************/

// Search the converter cache for a converter that says it can handle obj
// raise an error and if more than one answers the call (there can be only one highlander)
I3ConverterPtr I3TableWriter::FindConverter(I3FrameObjectConstPtr obj) {
    I3ConverterPtr converter_ptr;
    std::vector<I3ConverterMillPtr>::const_iterator it_conv;
    // Look for an exact match
    log_trace("Searching for an exact conversion");
    for(it_conv = converterCache_.begin(); it_conv != converterCache_.end(); it_conv++) {
        I3ConverterPtr conv = (*(*it_conv))();
        log_trace("Asking converter '%s' what it thinks of '%s'",name_of(conv).c_str(),name_of(obj).c_str());
        bool match = conv->CanConvert(obj)==I3Converter::ExactConversion;
        if (match && (converter_ptr != NULL)) {
            log_fatal("Ambiguity in the converter registry. Converters '%s' and '%s' both want to handle '%s'",
                        name_of(converter_ptr).c_str(),name_of(conv).c_str(),name_of(obj).c_str());
        } else if (match) {
            log_trace("Converter '%s' can exactly convert '%s'",name_of(conv).c_str(),name_of(obj).c_str());
            converter_ptr = conv;
        }
    }
    // If we found a direct match, return it.
    if(converter_ptr)
        return converter_ptr;
    // Otherwise, look for an inexact match
    log_trace("Searching for an inexact conversion");
    for(it_conv = converterCache_.begin(); it_conv != converterCache_.end(); it_conv++) {
        I3ConverterPtr conv = (*(*it_conv))();
        log_trace("Asking converter '%s' what it thinks of '%s'",name_of(conv).c_str(),name_of(obj).c_str());
        bool match = conv->CanConvert(obj)==I3Converter::InexactConversion;
        if (match && (converter_ptr != NULL)) {
            log_fatal("Ambiguity in the converter registry. Converters '%s' and '%s' both want to handle '%s'",
                      name_of(converter_ptr).c_str(),name_of(conv).c_str(),name_of(obj).c_str());
        } else if (match) {
            log_trace("Converter '%s' can inexactly convert '%s'",name_of(conv).c_str(),name_of(obj).c_str());
            converter_ptr = conv;
        }
    }
    return converter_ptr;
}

/******************************************************************************/

// register one specific object, lazily. if type and converter are empty the writer 
// should figure out appropriate values
bool I3TableWriter::AddObject(std::string name, std::string tableName, I3Frame::Stream frame_stop,
                              I3ConverterPtr converter, I3FrameObjectConstPtr obj) {

    // create a converter if needed
    if (converter == NULL) {
      log_debug("AddObject called for object %s table %s without a converter",
                 name.c_str(), tableName.c_str());
      converter = FindConverter(obj);
      if (converter == NULL) {
         log_fatal("No converter found for '%s' of type '%s'",name.c_str(),name_of(obj).c_str());
      }
     } else {
      log_debug("AddObject called for object '%s' table '%s' with a converter",
                 name.c_str(), tableName.c_str());
      // check the converter anyhow
      if (!converter->CanConvert(obj)) {
         log_fatal("Converter for key '%s' of type '%s' can't convert the object.",name.c_str(),name_of(obj).c_str());
      }
    }

    if (frame_stop != converter->GetStop()){
      return false;
    }
      
    // check to see whether the object contains anything at all
    // if it's empty, (e.g. an empty DOMLaunchSeriesMap, the converter 
    // might not have the information it needs to build the description)
    size_t nrows = converter->GetNumberOfRows(obj);
    // we assume that the converter needs a live object to build its description
    // if the object is empty, there's no harm in delaying instantiation of the table
    if (nrows == 0) return false;
    
    // construct the table description
    I3TableRowDescription combinedDescription;
    I3TableRowDescriptionConstPtr convDescription = converter->GetDescription(obj);

    if (frame_stop == I3Frame::Physics){
      I3TableRowDescriptionConstPtr ticDescription  = ticConverter_->GetDescription();
      combinedDescription = (*ticDescription | *convDescription);
    }
    else{
      combinedDescription = *convDescription;
      combinedDescription.SetUseIndex(false);
      combinedDescription.SetUsePadding(false);      
    }
    
    if (tableName == "")
        tableName = name;

    std::map<std::string,std::vector<TableBundle> >::iterator tlist_it = tables_.find(name);
    std::vector<TableBundle>::iterator t_it;
    bool duplicate = false;
    if (tlist_it != tables_.end()) {
       // a table is connected to the key already,
       // search for duplicates
       for (t_it = tlist_it->second.begin(); t_it != tlist_it->second.end(); ++t_it) {
          if (t_it->table->GetName() == tableName) {
             log_warn("There is already a table named '%s', appending superfluous underscores.",t_it->table->GetName().c_str());
             tableName += "_";
          }
          // by convention, identical description => identical converter
          if (combinedDescription == t_it->table->GetDescription()) {
             duplicate = true;
             log_error("Another table '%s' exists with the same description. The new table '%s' will be dropped.",
                t_it->table->GetName().c_str(),tableName.c_str());
             break;
          }
       }
    } else {
       tables_[name] = std::vector<TableBundle>();
       tlist_it = tables_.find(name);
    }
       
    if (!duplicate) {
      // get the table from the service
       log_debug("connecting object %s with table %s (%zu fields %s .. %s)", name.c_str(), tableName.c_str(), 
                 combinedDescription.GetNumberOfFields(),
                 combinedDescription.GetFieldNames().at(0).c_str(),
                 combinedDescription.GetFieldNames().at(combinedDescription.GetNumberOfFields()-1).c_str()); 
       I3TablePtr table = ConnectTable(tableName, combinedDescription);

       // store all this in tables_ 
       TableBundle bundle;
       bundle.converter = converter;
       bundle.table = table;
   
      tlist_it->second.push_back(bundle);
   }
   
   // the table was successfully added
   return true;
}


/******************************************************************************/
        
void I3TableWriter::AddObject(std::string objName, TableSpec spec) {
	tablespec_map::iterator n_it = wantedNames_.find(objName);
	if (n_it == wantedNames_.end()) {
		wantedNames_[objName] = std::vector<TableSpec>(1,spec);
	} else {
		n_it->second.push_back(spec);
	}
}

/******************************************************************************/

void I3TableWriter::AddType(TypeSpec type, TableSpec spec) {
	typespec_map::iterator t_it = wantedTypes_.find(type);
	if (t_it == wantedTypes_.end()) {
		wantedTypes_[type] = std::vector<TableSpec>(1,spec);
	} else {
		t_it->second.push_back(spec);
	}
}

/******************************************************************************/
        
void I3TableWriter::AddConverter(std::string typeName, I3ConverterPtr converter) {
	converters_[typeName] = converter;
}

/******************************************************************************/

void I3TableWriter::Setup() {
    // implemenation pending
}

/******************************************************************************/

const std::string I3TableWriter::GetTypeName(I3FramePtr frame, const std::string& key) {
	std::string typeName;
	try {
		typeName = I3::name_of(*frame->type_id(key));
	} catch (...) {
		typeName = "";
	}
	return typeName;
}

void I3TableWriter::Convert(I3FramePtr frame) {
    /*
    1. if types configured get list of object names and types from the frame
    2. for all configured names:
        convert
        (remove from list if list.size > 0)
    3. if list.size > 0 and types.size > 0:
        look in remaining objects if type fits
        add to configured objects
        convert
    */

  I3Frame::Stream frame_stop = frame->GetStop();

  if (frame_stop == I3Frame::DAQ){
    return;
  }

  I3EventHeaderConstPtr header;
  
  if ( frame_stop ==I3Frame::Physics){

    header = frame->Get<I3EventHeaderConstPtr>(); // the name is _not_ canonical
    if (!header) {
       log_error("This frame is missing an I3EventHeader and will not be booked!");
       return;
    }
    bool handle_stream = false;
    const std::string &stream = header->GetSubEventStream();
    BOOST_FOREACH(const std::string &target_stream, streams_)
        if (target_stream == stream) {
            handle_stream = true;
            break;
        }

    if (!handle_stream) {
        ignoredStreams_.insert(stream);
        return;
    }

    // lazily initialize the header
    if (!ticConverter_->HasDescription()) { ticConverter_->GetDescription(header); }
  }

    tablespec_map::iterator vlist_it;
    typespec_map::iterator typelist_it;
    std::vector<TableSpec>::iterator v_it;
    std::vector<std::string>::const_iterator k_it; 
    std::map<std::string, std::vector<TableBundle> >::iterator tlist_it;
    std::vector<TableBundle>::iterator t_it;
    std::map<std::string, std::string>::iterator m_it;

    // loop over the wantedNames_ list and move objects that are in the frame 
    // to tables_. this loop erases elements from wantedNames_
    for(vlist_it = wantedNames_.begin(); vlist_it != wantedNames_.end(); ) {
        const std::string& objName = vlist_it->first;
        tablespec_map::iterator eraser;          

        // try to get the object from the frame 
        // if exist: derive type, get converter, add to tables_, pop from wanted list
        I3FrameObjectConstPtr object;
        
        try {
           object = GetFrameObject(frame, objName);
        } catch (...) {
           log_error("Frame object '%s' could not be deserialized and will not be booked.",objName.c_str());
           eraser = vlist_it++;
           wantedNames_.erase(eraser);
           continue;
        }
        
        if (object) {
           bool success = false;
           for (v_it = vlist_it->second.begin(); v_it != vlist_it->second.end(); ) {
             
              success = AddObject(objName, v_it->tableName, frame_stop, v_it->converter, object);
              // if the TableSpec was successfully added, erase it from the queue
              // and move the to next element
              if (success) { 
                  v_it = vlist_it->second.erase(v_it);
              } else { // otherwise, just move to the next element
                  ++v_it;
              }
           }
           
           // if all the TableSpecs were added, remove the key from the queue
           if (vlist_it->second.size() == 0) {
               // copy the iterator to avoid invalidating it on erase
               eraser = vlist_it++;
               wantedNames_.erase(eraser);
           } else {
               ++vlist_it;
           } /* end if (vlist_it->second.size() == 0) */
       } else {
           ++vlist_it;
       } /* end if (object) */
       
     } /* end for(vlist_it) */

    // now, cycle through the typelist and look for objects in the frames that 
    // have the type and are not in tables_
    const std::vector<std::string>& objectsInFrame = frame->keys();

   // if the user specified types to be booked, loop through the frame's keys
   if (wantedTypes_.size() > 0) {
      // .. loop through the frame
      for (k_it = objectsInFrame.begin(); k_it != objectsInFrame.end(); ++k_it) {
         bool selected = false;
         const std::string& objName = *k_it;
         
         // skip this key if we've already examined it before
         // and decided we don't need it        
         if (uselessKeys_.find(objName) != uselessKeys_.end()) {  
            log_trace("Skipping examination of key '%s'",objName.c_str());
            continue;
         }
         
         // if the object is already in tables ignore it 
         if ( tables_.find(objName) != tables_.end() ) {
            uselessKeys_.insert(objName);
            log_trace("Added key '%s' to ban list (already booked)",objName.c_str());
            continue;
         }
         
         I3FrameObjectConstPtr object;
         
	 object = GetFrameObject(frame, objName);
	 if(!object && frame->Has(objName)){
            uselessKeys_.insert(objName);
            log_trace("Added key '%s' to ban list (unregistered class)",objName.c_str());
            continue;
         }

         // for every type in wantedTypes_ ...
         for (typelist_it = wantedTypes_.begin(); typelist_it != wantedTypes_.end(); ++typelist_it) {
             const TypeSpec& typeSpec = typelist_it->first;
               
             log_trace("Checking type of '%s'",objName.c_str());
             if ( typeSpec.check(object) ) {
                selected = true;
                for (v_it = typelist_it->second.begin(); v_it != typelist_it->second.end(); v_it++) {
                  AddObject(objName, v_it->tableName, frame_stop, v_it->converter, object);
                }
             } // if (typeName == objTypeName)
         } // for (typelist_it

         if (!selected) {
             uselessKeys_.insert(objName);
             log_trace("Added key '%s' to ban list (nobody wants it)",objName.c_str());
         } // if !selected
       } // for (k_it
    } // if wantedTypes_.size() > 0

    // now walk through tables_ and convert what is there
    for(tlist_it = tables_.begin(); tlist_it != tables_.end(); ++tlist_it) {  
        for(t_it = tlist_it->second.begin(); t_it!= tlist_it->second.end(); ++t_it) {
	
            const std::string& objName = tlist_it->first;
            const TableBundle& bundle = *t_it;

            if (bundle.converter->GetStop() != frame_stop){
              continue;
            }
            
            size_t nrows = 0;
            
            log_debug("converting object %s with converter %s to table %s", objName.c_str(), 
                      name_of(bundle.converter).c_str(), bundle.table->GetName().c_str());
            log_debug("(%zu fields %s .. %s)",  
                      bundle.table->GetDescription()->GetNumberOfFields(),
                      bundle.table->GetDescription()->GetFieldNames().at(0).c_str(),
                      bundle.table->GetDescription()->GetFieldNames().at(bundle.table->GetDescription()->GetNumberOfFields()-1).c_str()); 
            I3FrameObjectConstPtr obj = GetFrameObject(frame, objName);
            if (!obj)
                continue;

            try {
                nrows = bundle.converter->GetNumberOfRows(obj);
            } catch (const std::bad_cast&) {
                log_fatal("The frame object '%s' has switched types since the"
                    " last time it was seen. This sort of behavior is"
                    " unsupported!", objName.c_str());
            }

            // ask the converter how many rows he will write
            // skip the object if there is nothing to be written
            if (nrows != 0) {
            
                // with this information the table can create the rows
                I3TableRowPtr rows = bundle.table->CreateRow(nrows);
            
                // the converter can then fill them
                size_t rowsWritten = bundle.converter->Convert(obj, rows, frame);
            
                // e.g. rowWritten == 0 -> exist = 0
                i3_assert(rowsWritten == nrows);
                
                if (frame_stop==I3Frame::Physics){
                  // fill the table index columns
                  for (size_t i=0; i < nrows; ++i) {
                    rows->SetCurrentRow(i);
                    ticConverter_->Convert(header, rows, frame);
                    rows->Set<bool>("exists", true);
                  }
                }
                
                // hand the rows back to the table
                bundle.table->AddRow(header, rows);                
            }
        } // for t_it
    } // for tlist_it


}

/******************************************************************************/

void I3TableWriter::Finish() {
    // disconnect from all tables
    std::map<std::string, std::vector<TableBundle> >::iterator list_it;
    std::vector<TableBundle>::iterator it;
    for(list_it = tables_.begin(); list_it != tables_.end(); ++list_it) {
        for(it = list_it->second.begin(); it != list_it->second.end(); ++it) {
            DisconnectTable(it->table);
        }
    }
    
    if (ignoredStreams_.size() > 0) {
        std::ostringstream buf1, buf2;
        BOOST_FOREACH(const std::string &stream, ignoredStreams_)
            buf1 << "'" << stream << "',";
        BOOST_FOREACH(const std::string &stream, streams_)
            buf2 << "'" << stream << "',";
        log_warn("%zu SubEventStreams [%s] were seen but not booked because"
            " they were not passed as part of the 'SubEventStreams' parameter of"
            " I3TableWriter (which was configured as [%s]). To book events"
            " from these streams, add them to the 'SubEventStreams' parameter of I3TableWriter.",
            ignoredStreams_.size(), buf1.str().c_str(), buf2.str().c_str());
    }

    /* Tell the table service to clean up disconnected tables and
     * close its files if appropriate. */
    service_->Finish();
}

/******************************************************************************/

I3TablePtr I3TableWriter::ConnectTable(std::string tableName, 
                                       const I3TableRowDescription& description) {
    log_debug("Connecting to table %s", tableName.c_str());
    I3TableRowDescriptionPtr descPointer = 
        I3TableRowDescriptionPtr(new I3TableRowDescription(description));
    I3TablePtr table = service_->GetTable(tableName, descPointer);
    if (!table) {
        log_fatal("couldn't get table %s from the service", tableName.c_str());
    }
    table->SetConnectedToWriter(true);
    return table;
}

/******************************************************************************/

void I3TableWriter::DisconnectTable(I3TablePtr& table) {
    log_debug("Disconnecting from table %s", table->GetName().c_str()); 
    table->Align();
    table->SetConnectedToWriter(false);
    table.reset();
}
