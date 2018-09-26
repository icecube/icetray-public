/**
 * 
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "dataclasses/payload/I3SuperDST.h"

#include "dataclasses/payload/I3SuperDSTTrigger.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "icetray/python/dataclass_suite.hpp"

namespace bp = boost::python;

I3RecoPulseSeriesMapPtr
unpack_pulses(const I3SuperDST &self)
{
	I3RecoPulseSeriesMapConstPtr pmap = self.Unpack();
	if (pmap)
		return I3RecoPulseSeriesMapPtr(new I3RecoPulseSeriesMap(*pmap));
	else
		return I3RecoPulseSeriesMapPtr();
}

I3TriggerHierarchyPtr
unpack_triggers(const I3SuperDSTTriggerSeries &self, const I3DetectorStatus &status)
{
	I3TriggerHierarchyConstPtr trigs = self.Unpack(status);
	if (trigs)
		return I3TriggerHierarchyPtr(new I3TriggerHierarchy(*trigs));
	else
		return I3TriggerHierarchyPtr();
}

void
register_I3SuperDST()
{
	bp::class_<I3SuperDST, I3SuperDSTPtr, bp::bases<I3FrameObject> >(
	    "I3SuperDST", bp::no_init)
		.def(bp::init<const I3RecoPulseSeriesMap &>(bp::args("pulses")))
		.def("unpack", &unpack_pulses, bp::args("self"), "Unpack the compressed event data "
		    "into I3RecoPulses.")
	        .def( freeze() )
		#define RO_PROPS (EncodedSizes)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3SuperDST, RO_PROPS)
		#undef RO_PROPS
		.def(bp::dataclass_suite<I3SuperDST>())
		;
	register_pointer_conversions<I3SuperDST>();
	
	bp::class_<I3SuperDSTTrigger >(
	    "I3SuperDSTTrigger", bp::no_init)
		.def(bp::init<const I3Trigger &, const I3DetectorStatus &>(bp::args("trigger","status")))
        .def(bp::init<uint32_t, uint32_t, uint32_t>(bp::args("trigger_key_index","startcode","lengthcode")))
		.add_property("time", &I3SuperDSTTrigger::GetTime)
		.add_property("length", &I3SuperDSTTrigger::GetLength)
		.def("GetTriggerKey", &I3SuperDSTTrigger::GetTriggerKey, bp::args("self", "status"),
             bp::return_value_policy<bp::copy_const_reference>(), "Get the trigger key for this trigger.")
		.def("SetTimeReference", &I3SuperDSTTrigger::SetTimeReference, bp::args("self", "trigger"), "Set the time reference.")
		.def(bp::operator_suite<I3SuperDSTTrigger>())    
	;
	
	bp::class_<I3SuperDSTTriggerSeries, I3SuperDSTTriggerSeriesPtr, bp::bases<I3FrameObject> >(
	    "I3SuperDSTTriggerSeries", bp::no_init)
		.def(bp::init<const I3TriggerHierarchy &, const I3DetectorStatus &>(bp::args("triggers"), "status"))
		.def("unpack", &unpack_triggers, bp::args("self", "status"), "Unpack the compressed event data "
		    "into I3Triggers.")
		.def(bp::dataclass_suite<I3SuperDSTTriggerSeries>())   
	;
	register_pointer_conversions<I3SuperDSTTriggerSeries>();
}

