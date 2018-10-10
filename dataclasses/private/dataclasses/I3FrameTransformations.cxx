/**
 *  $Id$
 *
 *  Specializations of I3Frame::Get() that perform format
 *  conversions behind the scenes.
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <boost/shared_ptr.hpp>
#include <icetray/I3Frame.h>
#include <icetray/I3Units.h>
#include <dataclasses/I3MapOMKeyMask.h>
#include <dataclasses/I3MapOMKeyUnion.h>
#include <dataclasses/I3RecoPulseSeriesMapApplySPECorrection.h>
#include <dataclasses/physics/I3RecoPulseSeriesMapCombineByModule.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/payload/I3SuperDST.h>
#include <dataclasses/payload/I3SuperDSTTrigger.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/status/I3DetectorStatus.h>

using boost::dynamic_pointer_cast;

static I3RecoPulseSeriesMapPtr HitsAsPulses(I3RecoHitSeriesMapConstPtr hits);

/*
 * Only a little bit evil: Specialize the I3Frame::Get() to apply
 * a mask behind the scenes. This lets client code treat masks
 * and SuperDST payloads (and masked SuperDST payloads)
 * just like I3RecoPulseSeriesMaps.
 */ 
template <>
I3RecoPulseSeriesMapConstPtr
I3Frame::Get(const std::string& name, void*, void*) const
{
	I3FrameObjectConstPtr focp = this->Get<I3FrameObjectConstPtr>(name);
	
	I3RecoPulseSeriesMapConstPtr pulses =
	    boost::dynamic_pointer_cast<const I3RecoPulseSeriesMap>(focp);
	
	if (!focp || pulses)
		return pulses;
	
	I3RecoPulseSeriesMapMaskConstPtr mask = 
	    boost::dynamic_pointer_cast<const I3RecoPulseSeriesMapMask>(focp);
	
	if (mask)
		return mask->Apply(*this); 
		
	I3RecoPulseSeriesMapUnionConstPtr uni = 
	    boost::dynamic_pointer_cast<const I3RecoPulseSeriesMapUnion>(focp);

	if (uni)
		return uni->Apply(*this);

	I3RecoPulseSeriesMapApplySPECorrectionConstPtr spe_shift = 
	    boost::dynamic_pointer_cast<const I3RecoPulseSeriesMapApplySPECorrection>(focp);

	if (spe_shift)
		return spe_shift->Apply(*this);
	
	{
		I3RecoPulseSeriesMapCombineByModuleConstPtr combined = 
		    boost::dynamic_pointer_cast<const I3RecoPulseSeriesMapCombineByModule>(focp);
		if (combined)
			return combined->Apply(*this);
	}
	
	I3SuperDSTConstPtr superdst = 
	    boost::dynamic_pointer_cast<const I3SuperDST>(focp);
	
	if (superdst)
		return superdst->Unpack();

	// Compatibility with old data
	I3RecoHitSeriesMapConstPtr hits = 
	    boost::dynamic_pointer_cast<const I3RecoHitSeriesMap>(focp);

	if (hits)
		return HitsAsPulses(hits);
	else
		return pulses;
}

static I3RecoPulseSeriesMapPtr
HitsAsPulses(I3RecoHitSeriesMapConstPtr hits)
{
	I3RecoPulseSeriesMapPtr pulses(new I3RecoPulseSeriesMap);

	for (I3RecoHitSeriesMap::const_iterator i = hits->begin();
	    i != hits->end(); i++) {
		I3RecoPulseSeries &ps = (*pulses)[i->first];
		
		for (I3RecoHitSeries::const_iterator j = i->second.begin();
		    j != i->second.end(); j++) {
			I3RecoPulse pulse;
			pulse.SetTime(j->GetTime());
			pulse.SetCharge(1.0 /* PE */);
			pulse.SetWidth(3.3 * I3Units::ns /* Avg. ATWD bin */);
			ps.push_back(pulse);
		}
	}

	return pulses;
}

/*
 * Similar subterfuge for I3TriggerHierarchy
 */
template <>
I3TriggerHierarchyConstPtr
I3Frame::Get(const std::string& name, void*, void*) const
{
	I3FrameObjectConstPtr focp = this->Get<I3FrameObjectConstPtr>(name);

	I3TriggerHierarchyConstPtr triggers =
	    boost::dynamic_pointer_cast<const I3TriggerHierarchy>(focp);
	if (triggers)
		return triggers;
	
	I3SuperDSTTriggerSeriesConstPtr sdst =
	    boost::dynamic_pointer_cast<const I3SuperDSTTriggerSeries>(focp);
	I3DetectorStatusConstPtr status =
	    this->Get<I3DetectorStatusConstPtr>();
	if (sdst && !status)
		log_fatal("Can't decode SuperDST triggers without an I3DetectorStatus!");
	else if (sdst && status)
		return sdst->Unpack(*status);
	
	return triggers;
}
