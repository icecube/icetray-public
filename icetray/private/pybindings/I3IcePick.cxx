#include <boost/python.hpp>
#include <icetray/I3Module.h>
#include <icetray/PythonModule.h>

using namespace boost::python;

struct I3IcePickPython {

  I3IcePickWrapper


};



void register_I3IcePick()
{
  class_<PythonModule, boost::shared_ptr<PythonModule>,
    boost::noncopyable>("I3Module", init<const I3Context&>())
    .def("Configure", &PythonModule::Configure)
    .def("Physics", &PythonModule::Physics)
    .def("Geometry", &PythonModule::Geometry)
    .def("Calibration", &PythonModule::Calibration)
    .def("DetectorStatus", &PythonModule::DetectorStatus)
    .def("AddParameter", &PythonModule::AddParameter)
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
