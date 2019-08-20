
#include <icetray/I3PODHolder.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/operator_suite.hpp>

namespace bp = boost::python;
typedef I3PODHolder<uint64_t> I3UInt64;

static std::string 
I3UInt64_prettyprint(const I3UInt64& d)
{
  std::ostringstream oss;
  oss << "I3UInt64(" << d.value << ")";
  return oss.str();
}

bool I3UInt64_bool(const I3UInt64& v)
{
    return v.value != uint64_t(0);
}

void register_I3UInt64() {
    bp::class_<I3UInt64, bp::bases<I3FrameObject>, boost::shared_ptr<I3UInt64> >("I3UInt64",
      "A serializable uint64_t. Can compare directly with numeric types.\n\
  Note that python assignment is by reference, creating two links to one object.")
      .def(bp::init<>())
      .def(bp::init<uint64_t>())
      .def(bp::init<const I3UInt64&>())
      .def_readwrite("value", &I3UInt64::value)
      .def("__repr__",I3UInt64_prettyprint)
      .def_pickle(bp::boost_serializable_pickle_suite<I3UInt64>())
      .def(bp::operator_suite<I3UInt64>())
      .def(bp::operator_int_suite<I3UInt64>())
      .def(bp::operator_float_suite<I3UInt64>())
      .def("__nonzero__", I3UInt64_bool)
      .def("__bool__", I3UInt64_bool)      
      .def( freeze() )
      ;

    register_pointer_conversions<I3UInt64>();
}
