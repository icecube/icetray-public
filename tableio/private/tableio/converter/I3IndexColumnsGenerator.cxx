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

#include "tableio/converter/I3IndexColumnsGenerator.h"
#include <boost/foreach.hpp>

I3IndexColumnsGenerator::I3IndexColumnsGenerator()
{
	streams_[""] = 0;
	istreams_.push_back("");
}

I3IndexColumnsGenerator::I3IndexColumnsGenerator(const std::vector<std::string> &streams)
{
	int num = 0;
	BOOST_FOREACH(const std::string &stream, streams) {
		streams_[stream] = num++;
		istreams_.push_back(stream);
	}
}

I3IndexColumnsGenerator::I3IndexColumnsGenerator(I3TableRowDescriptionConstPtr desc)
{
	size_t idx = desc->GetFieldColumn("SubEventStream");
	if (idx == desc->GetNumberOfFields())
		return;

	const I3Datatype &dtype = desc->GetFieldTypes()[idx];

	
	typedef std::pair<std::string,long> enum_map_t;
	BOOST_FOREACH(const enum_map_t &pair, dtype.enum_members)
		streams_[pair.first] = pair.second;
	BOOST_FOREACH(const stream_map_t::value_type &pair, streams_)
		istreams_.push_back(pair.first);
}

I3TableRowDescriptionPtr
I3IndexColumnsGenerator::CreateDescription(const I3EventHeader& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );

	/* You want something done right, you got to do it yourself. */
	I3Datatype enum_type;
	enum_type.size = sizeof(int32_t);
	enum_type.kind = I3Datatype::Enum;
	enum_type.is_signed = true;
	enum_type.description = "";
	
	/* XXX HACK: Replace "" with "NULL" in the enum description */ 
	BOOST_FOREACH(const stream_map_t::value_type &pair, streams_)
		if (pair.first.size() == 0)
			enum_type.enum_members.push_back(
			    std::make_pair(std::string("NULL"), pair.second));
		else
			enum_type.enum_members.push_back(pair);

            desc->AddField<uint32_t>("Run", "", "run number");
            desc->AddField<uint32_t>("Event", "", "event number");
            desc->AddField<uint32_t>("SubEvent", "", "sub-event number");
            desc->AddField("SubEventStream", enum_type, "",
                "ID of the splitter module that made this stream", 1);
            desc->AddField<bool>("exists", "", "object was found in the frame");
            return desc;
        }

        size_t
I3IndexColumnsGenerator::FillRows(const I3EventHeader& header, I3TableRowPtr rows) {
            rows->Set<uint32_t>("Run", header.GetRunID());
            rows->Set<uint32_t>("Event", header.GetEventID());
            rows->Set<uint32_t>("SubEvent", header.GetSubEventID());
            rows->SetEnumsAreInts(true); /* Disconnect seatbelt */
	// printf("Stream %s: %d\n", header.GetSubEventStream().c_str(), streams_[header.GetSubEventStream()]);
            rows->Set<int32_t>("SubEventStream", streams_[header.GetSubEventStream()]);
            rows->Set<bool>("exists", false); // to be overwritten by someone else, e.g. I3TableWriter
            return 1;
        }

I3EventHeaderPtr
I3IndexColumnsGenerator::Resurrect(I3TableRowPtr rows) {
	I3EventHeaderPtr header(new I3EventHeader());
	header->SetRunID(rows->Get<uint32_t>("Run"));
	header->SetEventID(rows->Get<uint32_t>("Event"));
	header->SetSubEventID(rows->Get<uint32_t>("SubEvent"));
	rows->SetEnumsAreInts(true); /* Disconnect seatbelt */
	int32_t idx = rows->Get<int32_t>("SubEventStream");
	header->SetSubEventStream(istreams_.at(idx));
	
	return header;
}
