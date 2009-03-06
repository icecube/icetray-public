#include <boost/python.hpp>
#include <icetray/I3Module.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/PythonModule.h>

using namespace boost::python;

namespace {
  template <typename Base>
  void 
  go(const char* name)
  {
    typedef PythonModule<Base> module_t;

    class_<module_t, boost::shared_ptr<module_t>, boost::noncopyable>(name, init<const I3Context&>())
      .def("Configure", &module_t::Configure)
      .def("Physics", &module_t::Physics)
      .def("Geometry", &module_t::Geometry)
      .def("Calibration", &module_t::Calibration)
      .def("DetectorStatus", &module_t::DetectorStatus)
      .def("Register", &module_t::Register)
      .def("AddParameter", 
	   (void (module_t::*)(const std::string&, const std::string&, const boost::python::object&))
	   &module_t::AddParameter)
      .def("AddParameter", 
	   (void (module_t::*)(const std::string&, const std::string&))
	   &module_t::AddParameter)
      .def("GetParameter", &module_t::GetParameter)
      .def("Finish", &module_t::Finish)
      .def("AddOutBox", &module_t::AddOutBox)
      .def("PushFrame", (void (module_t::*)(I3FramePtr)) &module_t::PushFrame)
      .def("PushFrame", (void (module_t::*)(I3FramePtr, const std::string&)) &module_t::PushFrame)
      .def("PopFrame", &module_t::PopFrame)
      .def("Process", &module_t::Process)
      ;

    implicitly_convertible<boost::shared_ptr<module_t>, boost::shared_ptr<I3Module> >();
  }
}

//void fleh(boost::python::object obj)
//{
//  log_error("PyType_Check: %d", PyType_Check(obj.ptr()));
//  log_error("PyEval_GetFuncName: %s", PyEval_GetFuncName(obj.ptr()));
//  log_error("PyEval_GetGlobals: %p", PyEval_GetGlobals());
//}

void register_I3Module()
{
  go<I3Module>("I3Module");
  go<I3ConditionalModule>("I3ConditionalModule");
}
