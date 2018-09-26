
#include <dataclasses/status/I3FlasherStatus.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3FlasherStatus() {

  class_<I3FlasherStatus, bases<I3FrameObject>,
         boost::shared_ptr<I3FlasherStatus> >("I3FlasherStatus")
  #define PROPS (brightness)(window)(rate)(mask)(delay)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3FlasherStatus, PROPS)
  #undef PROPS
  .def(dataclass_suite<I3FlasherStatus>())
  ;

  register_pointer_conversions<I3FlasherStatus>();
};

void register_I3FlasherStatusMap() {

  class_<I3FlasherStatusMap, bases<I3FrameObject>,
      boost::shared_ptr<I3FlasherStatusMap> >("I3FlasherStatusMap")
  .def(dataclass_suite<I3FlasherStatusMap>())
  ;

  register_pointer_conversions<I3FlasherStatusMap>();
}

void register_I3FlasherSubrunMap() {

  class_<I3FlasherSubrunMap, bases<I3FrameObject>,
      boost::shared_ptr<I3FlasherSubrunMap> >("I3FlasherSubrunMap")
  .def(dataclass_suite<I3FlasherSubrunMap>())
  ;

  register_pointer_conversions<I3FlasherSubrunMap>();
}
