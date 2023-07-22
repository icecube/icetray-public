#include <phys-services/MultiPMTCoincify.h>
#include <boost/python.hpp>

namespace bp=boost::python;
void register_MultiPMTCoincify(){
  bp::class_<MultiPMTCoincify, boost::shared_ptr<MultiPMTCoincify>, boost::noncopyable>
    ("MultiPMTCoincify", bp::init<unsigned int, double, double, bool>(
      (bp::arg("span"), bp::arg("time_between_modules"), bp::arg("time_between_pmts"), bp::arg("reset_lc")=true)))
     .def("Coincify", &MultiPMTCoincify::Coincify, bp::args("pulses"))
     .def_readwrite("span", &MultiPMTCoincify::moduleSpan_)
     .def_readwrite("time_between_modules", &MultiPMTCoincify::moduleTime_)
     .def_readwrite("time_between_pmts", &MultiPMTCoincify::pmtTime_)
     .def_readwrite("reset_lc", &MultiPMTCoincify::reset_)
    ;
}
