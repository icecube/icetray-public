/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include "dataclasses/I3MapOMKeyUnion.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "boost/make_shared.hpp"
#include "boost/foreach.hpp"

I3RecoPulseSeriesMapUnion::I3RecoPulseSeriesMapUnion() : keys_(), unified_() {}

I3RecoPulseSeriesMapUnion::I3RecoPulseSeriesMapUnion(const I3Frame &frame,
    const std::vector<std::string> &keys) : keys_(keys), unified_() {}

static bool PulseCompare(const I3RecoPulse &p1, const I3RecoPulse &p2)
{
	return p1.GetTime() < p2.GetTime();
}

I3RecoPulseSeriesMapConstPtr
I3RecoPulseSeriesMapUnion::Apply(const I3Frame &frame) const
{
	typedef I3RecoPulseSeriesMap Map;
	typedef boost::shared_ptr<const Map> MapConstPtr;
	typedef Map::value_type Pair;
	typedef Pair::second_type Series;
	typedef Series::value_type Element;
	
	if (unified_)
		return unified_;
	
	unified_ = boost::make_shared<Map>();
	
	BOOST_FOREACH(const std::string &key, keys_) {
		MapConstPtr pmap = frame.Get<MapConstPtr>(key);
		if (!pmap)
			log_fatal("Couldn't find '%s' in the frame!",
			    key.c_str());
		
		BOOST_FOREACH(const Pair &pair, *pmap) {
			Series &univec = (*unified_)[pair.first];
			BOOST_FOREACH(const Element &element, pair.second)
				univec.push_back(element);
		}
	}
	
	BOOST_FOREACH(Pair &pair, *unified_)
		std::sort(pair.second.begin(), pair.second.end(),
		    PulseCompare);
	
	return unified_;
}

bool
I3RecoPulseSeriesMapUnion::operator==(const I3RecoPulseSeriesMapUnion& other) const
{
	return keys_ == other.keys_;
}

bool
I3RecoPulseSeriesMapUnion::operator!=(const I3RecoPulseSeriesMapUnion& other) const
{
	return keys_ != other.keys_;
}

template <class Archive>
void
I3RecoPulseSeriesMapUnion::serialize(Archive& ar, unsigned version)
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("Keys", keys_);
}

std::ostream& I3RecoPulseSeriesMapUnion::Print(std::ostream& oss) const{
	oss << "I3RecoPulseSeriesMapUnion Keys:";
	for(const auto& key : keys_)
		oss << "\n  " << key;
	return oss;
}

std::ostream& operator<<(std::ostream& os, const I3RecoPulseSeriesMapUnion& un){
	return(un.Print(os));
}

I3_SERIALIZABLE(I3RecoPulseSeriesMapUnion);
