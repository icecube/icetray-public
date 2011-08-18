#include <boost/python.hpp>
#include <icetray/I3Module.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3PacketModule.h>
#include <icetray/PythonModule.h>

using namespace boost::python;

namespace {
  #define REGMODMETHODS \
      .def("Configure", &module_t::Configure) \
      .def("ShouldDoDAQ", &module_t::ShouldDoDAQ) \
      .def("DAQ", &module_t::DAQ) \
      .def("ShouldDoPhysics", &module_t::ShouldDoPhysics) \
      .def("Physics", &module_t::Physics) \
      .def("ShouldDoGeometry", &module_t::ShouldDoGeometry) \
      .def("Geometry", &module_t::Geometry) \
      .def("ShouldDoCalibration", &module_t::ShouldDoCalibration) \
      .def("Calibration", &module_t::Calibration) \
      .def("ShouldDoDetectorStatus", &module_t::ShouldDoDetectorStatus) \
      .def("DetectorStatus", &module_t::DetectorStatus) \
      .def("Register", &module_t::Register) \
      .def("AddParameter", \
	   (void (module_t::*)(const std::string&, const std::string&, const boost::python::object&)) \
	   &module_t::AddParameter) \
      .def("AddParameter", \
	   (void (module_t::*)(const std::string&, const std::string&)) \
	   &module_t::AddParameter) \
      .def("GetParameter", &module_t::GetParameter) \
      .def("Finish", &module_t::Finish) \
      .def("AddOutBox", &module_t::AddOutBox) \
      .def("PushFrame", (void (module_t::*)(I3FramePtr)) &module_t::PushFrame) \
      .def("PushFrame", (void (module_t::*)(I3FramePtr, const std::string&)) &module_t::PushFrame) \
      .def("PopFrame", &module_t::PopFrame) \
      .def("Process", &module_t::Process) \
      .def("RequestSuspension",&module_t::RequestSuspension) \


  template <typename Base>
  void 
  wrapmod(const char *name)
  {
    typedef PythonModule<Base> module_t;

    class_<module_t, boost::shared_ptr<module_t>, boost::noncopyable>(name, init<const I3Context&>())
      REGMODMETHODS;

    implicitly_convertible<boost::shared_ptr<module_t>, boost::shared_ptr<I3Module> >();
  }

  template <typename Base>
  void 
  wrapderivedmod(const char *name)
  {
    typedef PythonModule<Base> module_t;

    class_<module_t, bases<PythonModule<I3Module> >,boost::shared_ptr<module_t>, boost::noncopyable>(name, init<const I3Context&>())
      REGMODMETHODS;

    implicitly_convertible<boost::shared_ptr<module_t>, boost::shared_ptr<I3Module> >();
  }

  template <>
  void 
  wrapderivedmod<I3PacketModule>(const char *name)
  {
    typedef PythonModule<I3PacketModule> module_t;

    class_<module_t, bases<PythonModule<I3Module> >,boost::shared_ptr<module_t>, boost::noncopyable>(name, init<const I3Context&, I3Frame::Stream>())
      REGMODMETHODS
      .def("FlushQueue", &module_t::FlushQueue)
      .def("FramePacket", &module_t::FramePacket)
      .add_property("sentinel",
           (I3Frame::Stream (module_t::*)())&module_t::GetSentinel,
           (void (module_t::*)(I3Frame::Stream))&module_t::SetSentinel)
      .add_property("packet_types",
           make_function((const std::vector<I3Frame::Stream>& (module_t::*)())
             &module_t::GetPacketTypes, return_internal_reference<1>()),
           (void (module_t::*)(const std::vector<I3Frame::Stream>&))
             &module_t::SetPacketTypes);

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
  wrapmod<I3Module>("I3Module");
  wrapderivedmod<I3ConditionalModule>("I3ConditionalModule");
  wrapderivedmod<I3PacketModule>("I3PacketModule");
}
