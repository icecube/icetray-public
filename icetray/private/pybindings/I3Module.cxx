#include <boost/python.hpp>
#include <icetray/I3Module.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3PacketModule.h>
#include <icetray/PythonModule.h>

using namespace boost::python;

namespace {
  #define REGMODMETHODS \
      .def("Configure", &module_t::PyConfigure) \
      .def("Register", &module_t::Register) \
      .def("AddParameter", \
	   (void (module_t::*)(const std::string&, const std::string&, const boost::python::object&)) \
	   &module_t::AddParameter) \
      .def("AddParameter", \
	   (void (module_t::*)(const std::string&, const std::string&)) \
	   &module_t::AddParameter) \
      .def("GetParameter", &module_t::GetParameter) \
      .def("Finish", &module_t::PyFinish) \
      .def("AddOutBox", &module_t::AddOutBox) \
      .def("PushFrame", (void (module_t::*)(I3FramePtr)) &module_t::PushFrame) \
      .def("PushFrame", (void (module_t::*)(I3FramePtr, const std::string&)) &module_t::PushFrame) \
      .def("PopFrame", &module_t::PopFrame) \
      .def("Process", &module_t::PyProcess) \
      .def("RequestSuspension",&module_t::RequestSuspension) \
      .def("ShouldDoGeometry", &module_t::ShouldDoGeometry) \
      .def("Geometry", &module_t::Geometry) \
      .def("ShouldDoCalibration", &module_t::ShouldDoCalibration) \
      .def("Calibration", &module_t::Calibration) \
      .def("ShouldDoDetectorStatus", &module_t::ShouldDoDetectorStatus) \
      .def("DetectorStatus", &module_t::DetectorStatus) \
      .def("ShouldDoSimulation", &module_t::ShouldDoSimulation)\
      .def("Simulation", &module_t::Simulation)\
      .def("ShouldDoDAQ", &module_t::ShouldDoDAQ) \
      .def("DAQ", &module_t::DAQ) \
      .def("ShouldDoPhysics", &module_t::ShouldDoPhysics) \
      .def("Physics", &module_t::Physics) \
      .add_property("configuration", make_function(&module_t::GetConfiguration, return_internal_reference<>())) \
      .add_property("name", &module_t::GetName) \
      .add_property("context", make_function(&module_t::GetContext, return_internal_reference<>())) \


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

  boost::shared_ptr<PythonModule<I3PacketModule> >
  simple_I3PacketModule(const I3Context& context)
  {
	return boost::shared_ptr<PythonModule<I3PacketModule> >(new PythonModule<I3PacketModule>(context));
  }

  template <>
  void 
  wrapderivedmod<I3PacketModule>(const char *name)
  {
    typedef PythonModule<I3PacketModule> module_t;

    class_<module_t, bases<PythonModule<I3Module> >,boost::shared_ptr<module_t>, boost::noncopyable>(name, init<const I3Context&, I3Frame::Stream>())
      REGMODMETHODS
      .def("__init__", make_constructor(simple_I3PacketModule))
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

void register_I3Module()
{
  wrapmod<I3Module>("I3Module");
  wrapderivedmod<I3ConditionalModule>("I3ConditionalModule");
  wrapderivedmod<I3PacketModule>("I3PacketModule");
}
