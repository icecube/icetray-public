#include "phys-services/I3XMLOMKey2MBID.h"

namespace bp = boost::python;

void register_I3XMLOMKey2MBID()
{
  bp::class_<I3XMLOMKey2MBID, boost::shared_ptr<I3XMLOMKey2MBID>, boost::noncopyable>
          ("I3XMLOMKey2MBID", bp::init<std::string>())
	  .def("dump", &I3XMLOMKey2MBID::Dump)
	  .staticmethod("dump")
	  .def("omkey_exists", &I3XMLOMKey2MBID::OMKeyExists)
	  .def("get_omkey", &I3XMLOMKey2MBID::GetOMKey)
	  .def("mbid_exists", &I3XMLOMKey2MBID::MBIDExists)
	  .def("get_mbid", &I3XMLOMKey2MBID::GetMBID)
 
	;
}
