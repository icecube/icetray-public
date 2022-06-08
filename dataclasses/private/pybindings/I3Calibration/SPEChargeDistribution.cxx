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

using namespace boost::python;

void register_SPEChargeDistribution()
{
  class_<SPEChargeDistribution>("SPEChargeDistribution")
    .def_readwrite("exp1_amp", 		&SPEChargeDistribution::exp1_amp)
    .def_readwrite("exp1_width", 	&SPEChargeDistribution::exp1_width)
    .def_readwrite("exp2_amp", 		&SPEChargeDistribution::exp2_amp)
    .def_readwrite("exp2_width", 	&SPEChargeDistribution::exp2_width)
    .def_readwrite("gaus_amp", 		&SPEChargeDistribution::gaus_amp)
    .def_readwrite("gaus_mean", 	&SPEChargeDistribution::gaus_mean)
    .def_readwrite("gaus_width", 	&SPEChargeDistribution::gaus_width)
    .def_readwrite("compensation_factor", 	&SPEChargeDistribution::compensation_factor)
    .def_readwrite("slc_gaus_mean", 	&SPEChargeDistribution::SLC_gaus_mean)
    .add_property("is_valid", 		&SPEChargeDistribution::IsValid)
    .def(dataclass_suite<SPEChargeDistribution>())
    ;
}
