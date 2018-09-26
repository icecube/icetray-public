#include <boost/python.hpp>
#include <phys-services/I3Splitter.h>

using namespace boost::python;

void register_I3Splitter()
{
	class_<I3Splitter, boost::noncopyable>("I3Splitter",
	    init<const I3Configuration&>())
		.def("get_next_sub_event", &I3Splitter::GetNextSubEvent);
}

