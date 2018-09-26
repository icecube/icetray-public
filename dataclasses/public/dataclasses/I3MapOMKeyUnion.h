/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#ifndef DATACLASSES_I3MAPOMKEYUNION_H_INCLUDED
#define DATACLASSES_I3MAPOMKEYUNION_H_INCLUDED

#include <functional>
#include <string>
#include <list>
#include "icetray/I3FrameObject.h"
#include "icetray/OMKey.h"
#include "icetray/I3Frame.h"
#include "icetray/serialization.h"
#include "dataclasses/physics/I3RecoPulse.h"

static const unsigned i3recopulseseriesmapunion_version_ = 0;

class I3RecoPulseSeriesMapUnion : public I3FrameObject {
public:
	/* 
	 * Construct a union for the map stored at "key." All bits are set.
	 */
	I3RecoPulseSeriesMapUnion(const I3Frame&, const std::vector<std::string> &keys);
	I3RecoPulseSeriesMapUnion();
	
	std::ostream& Print(std::ostream&) const override;
	
	I3RecoPulseSeriesMapConstPtr Apply(const I3Frame&) const;
	std::vector<std::string> GetSources() const { return keys_; }
    
	bool operator==(const I3RecoPulseSeriesMapUnion&) const;
	bool operator!=(const I3RecoPulseSeriesMapUnion&) const;
private:
	std::vector<std::string> keys_;
	mutable I3RecoPulseSeriesMapPtr unified_;
	
	friend class icecube::serialization::access;
	template <class Archive> void serialize(Archive& ar, unsigned version);
	
	SET_LOGGER("I3RecoPulseSeriesMapUnion");
};

std::ostream& operator<<(std::ostream&, const I3RecoPulseSeriesMapUnion&);

I3_CLASS_VERSION(I3RecoPulseSeriesMapUnion, i3recopulseseriesmapunion_version_);
I3_POINTER_TYPEDEFS(I3RecoPulseSeriesMapUnion);

#endif
