
#include "dataclasses/I3TimeWindow.h"
#include "icetray/python/dataclass_suite.hpp"

void
register_I3TimeWindow()
{
	bp::class_<I3TimeWindow, I3TimeWindowPtr,
	    bp::bases<I3FrameObject> >("I3TimeWindow", bp::init<double, double>())
	    .def(bp::init<>())
	    .def(bp::dataclass_suite<I3TimeWindow>())
	    #define RO_PROPS (Start)(Stop)(Length)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3TimeWindow, RO_PROPS)
	    #undef RO_PROPS
	    .def("__contains__", &I3TimeWindow::Contains)
	;
	
	bp::class_<I3TimeWindowSeries, I3TimeWindowSeriesPtr,
	    bp::bases<I3FrameObject> >("I3TimeWindowSeries")
	    .def(bp::dataclass_suite<I3TimeWindowSeries>())
	;
	
	bp::class_<I3TimeWindowSeriesMap, I3TimeWindowSeriesMapPtr,
	    bp::bases<I3FrameObject> >("I3TimeWindowSeriesMap")
	    .def(bp::dataclass_suite<I3TimeWindowSeriesMap>())
	    .def(bp::self | bp::self) // odd syntax for resolving operators, no matter how they're defined 
	;
	
	register_pointer_conversions<I3TimeWindow>();
	register_pointer_conversions<I3TimeWindowSeries>();
	register_pointer_conversions<I3TimeWindowSeriesMap>();
}
