#include <icetray/I3PhysicsUsage.h>

using namespace boost::python;

void
register_I3PhysicsUsage()
{
  class_<I3PhysicsUsage>("I3PhysicsUsage")
    .def_readwrite("systime", &I3PhysicsUsage::systime)
    .def_readwrite("usertime", &I3PhysicsUsage::usertime)
    .def_readwrite("ncall", &I3PhysicsUsage::ncall)
    .def(self_ns::str(self))
    ;

  class_<std::map<std::string, I3PhysicsUsage> >("map_string_I3PhysicsUsage")
    .def(map_indexing_suite<std::map<std::string, I3PhysicsUsage> >())
    ;

}
