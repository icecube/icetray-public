#include <boost/python.hpp>
#include <icetray/I3Module.h>
#include <icetray/PythonModule.h>

using namespace boost::python;

void register_I3Module()
{
  class_<PythonModule, boost::shared_ptr<PythonModule>, 
    boost::noncopyable>("I3Module", init<const I3Context&>())
    .def("Configure", &PythonModule::Configure)
    .def("Physics", &PythonModule::Physics)
    .def("Geometry", &PythonModule::Geometry)
    .def("Calibration", &PythonModule::Calibration)
    .def("DetectorStatus", &PythonModule::DetectorStatus)
    .def("Register", &PythonModule::Register)
    .def("AddParameter", 
	 (void (PythonModule::*)(const std::string&, const std::string&, const boost::python::object&))
	 &PythonModule::AddParameter)
    .def("AddParameter", 
	 (void (PythonModule::*)(const std::string&, const std::string&))
	 &PythonModule::AddParameter)
    .def("GetParameter", &PythonModule::GetParameter)
    .def("Finish", &PythonModule::Finish)
    .def("AddOutBox", &PythonModule::AddOutBox)
    .def("PushFrame", (void (PythonModule::*)(I3FramePtr)) &PythonModule::PushFrame)
    .def("PushFrame", (void (PythonModule::*)(I3FramePtr, const std::string&)) &PythonModule::PushFrame)
    .def("PopFrame", &PythonModule::PopFrame)
    .def("Process", &PythonModule::Process)
    ;
  implicitly_convertible<boost::shared_ptr<PythonModule>, boost::shared_ptr<I3Module> >();

}
