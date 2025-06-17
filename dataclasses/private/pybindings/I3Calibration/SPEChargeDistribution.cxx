//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: LinearFit.cxx 122591 2014-08-19 02:28:30Z david.schultz $
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//
#include <dataclasses/calibration/I3DOMCalibration.h>
#include <icetray/python/dataclass_suite.hpp>
#include <icetray/python/list_indexing_suite.hpp>

using namespace boost::python;

using PDF = SPEChargeDistribution::PDF;
using PDFPtr = boost::shared_ptr<SPEChargeDistribution::PDF>;
using Gaussian = SPEChargeDistribution::Gaussian;
using Exponential = SPEChargeDistribution::Exponential;
using vectorPDF = std::vector<PDFPtr>;
using Residuals = SPEChargeDistribution::Residuals;

static bool element_equality(const PDFPtr& a, const PDFPtr& b){
  bool equal = true;
  if(typeid(a)!=typeid(b))
    equal = false;
  else if(a->IsGaussian()){
    Gaussian* gaus_a = dynamic_cast<Gaussian*>(a.get());
    Gaussian* gaus_b = dynamic_cast<Gaussian*>(b.get());
    equal = *gaus_a==*gaus_b;
  }
  else{
    Exponential* expo_a = dynamic_cast<Exponential*>(a.get());
    Exponential* expo_b = dynamic_cast<Exponential*>(b.get());
    equal = *expo_a==*expo_b;
  }
  return equal;
}

bool vector_equality(const vectorPDF& a, const vectorPDF& b){
  return std::equal(a.begin(), a.end(), b.begin(), element_equality);
}

void pyset_residuals(Residuals& self, list x, list y) {
  stl_input_iterator<double> xbegin(x), ybegin(y), end;
  std::vector<double> xvec((stl_input_iterator<double>(x)),
                           (stl_input_iterator<double>()));
  std::vector<double> yvec((stl_input_iterator<double>(y)),
                           (stl_input_iterator<double>()));
  self.Set(xvec, yvec);
}

Residuals residuals_from_py(list x, list y){
  Residuals out;
  pyset_residuals(out, x, y);
  return out;
}

void register_SPEChargeDistribution()
{
  class_<PDF, PDFPtr, boost::noncopyable>("PDF", no_init)
    .def(dataclass_suite<PDF>());

  class_<vectorPDF>("vectorPDF")
    .def(list_indexing_suite<vectorPDF, true>())
    .def("__eq__", &vector_equality);

 scope spe = class_<SPEChargeDistribution, boost::shared_ptr<SPEChargeDistribution>>("SPEChargeDistribution")
    .def_readwrite("compensation_factor", &SPEChargeDistribution::compensation_factor)
    .def_readwrite("fadc_charge_scale",   &SPEChargeDistribution::fadc_charge_scale)
    .def_readwrite("pdfs",                &SPEChargeDistribution::PDFs)
    .def_readwrite("residuals",           &SPEChargeDistribution::residuals)
    .add_property("is_valid", 	          &SPEChargeDistribution::IsValid)
    .def(self == self)
    .def(self != self)
    .def("__call__",                      &SPEChargeDistribution::operator())
    .def("__str__",                       &SPEChargeDistribution::Print)
    .def("Mean",                          &SPEChargeDistribution::Mean)
    .def("StdDev",                        &SPEChargeDistribution::StdDev)
    .def(dataclass_suite<SPEChargeDistribution>())
    ;

  class_<Gaussian, boost::shared_ptr<Gaussian>, bases<PDF>>("Gaussian")
    .def(init<double,double,double>(args("amplitude", "mean", "sigma")))
    .def_readwrite("amplitude",    &Gaussian::amplitude_)
    .def_readwrite("mean",         &Gaussian::mean_)
    .def_readwrite("sigma",        &Gaussian::sigma_)
    .add_property("relative_norm", &Gaussian::RelativeNormalization)
    .def("probability",            &Gaussian::Probability, arg("charge"))
    .def("__eq__",                 &Gaussian::operator==)
    .def("__str__",                &Gaussian::Print)
    .def(dataclass_suite<Gaussian>())
    ;

  class_<Exponential, boost::shared_ptr<Exponential>, bases<PDF>>("Exponential")
    .def(init<double,double>(args("amplitude", "width")))
    .def_readwrite("amplitude",    &Exponential::amplitude_)
    .def_readwrite("width",        &Exponential::width_)
    .add_property("relative_norm", &Exponential::RelativeNormalization)
    .def("probability",            &Exponential::Probability, arg("charge"))
    .def("__eq__",                 &Exponential::operator==)
    .def("__str__",                &Exponential::Print)
    .def(dataclass_suite<Exponential>())
    ;
  
  class_<Residuals, boost::shared_ptr<Residuals>>("Residuals")
    //.def("__init__",               make_constructor(&residuals_from_py))
    .def("Set",                    &pyset_residuals)
    .def("SetPass2",               &Residuals::SetPass2)
    .def_readonly("x",             &Residuals::GetX)
    .def_readonly("y",             &Residuals::GetY)
    .def("GetMaxResidual",         &Residuals::GetMaxResidual)
    .def("ComputeResidual",        &Residuals::ComputeResidual)
    ;
}
