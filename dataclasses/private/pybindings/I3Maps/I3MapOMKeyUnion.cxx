
#include <dataclasses/I3MapOMKeyUnion.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

I3RecoPulseSeriesMapPtr
underhanded_apply(const I3RecoPulseSeriesMapUnion& mask, const I3Frame &frame)
{
	return boost::const_pointer_cast<I3RecoPulseSeriesMap>(mask.Apply(frame));
}

void register_I3RecoPulseSeriesMapUnion()
{
	bp::class_<I3RecoPulseSeriesMapUnion, bp::bases<I3FrameObject>,
	    I3RecoPulseSeriesMapUnionPtr>("I3RecoPulseSeriesMapUnion",
	    bp::init<const I3Frame&, const std::vector<std::string> &>(bp::args("frame", "keys")))
		.def("apply", &underhanded_apply, "Apply the union to an I3Frame, returning an I3RecoPulseSeries.")
		.add_property("sources", &I3RecoPulseSeriesMapUnion::GetSources)
		.def(bp::dataclass_suite<I3RecoPulseSeriesMapUnion>())
	    ;
	;
	
	register_pointer_conversions<I3RecoPulseSeriesMapUnion>();
}
