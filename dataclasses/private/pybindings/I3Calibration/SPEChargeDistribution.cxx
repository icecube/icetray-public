//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: LinearFit.cxx 122591 2014-08-19 02:28:30Z david.schultz $
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <dataclasses/calibration/I3DOMCalibration.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_SPEChargeDistribution()
{
  class_<SPEChargeDistribution>("SPEChargeDistribution")
    .def_readwrite("exp_amp", &SPEChargeDistribution::exp_amp)
    .def_readwrite("exp_width", &SPEChargeDistribution::exp_width)
    .def_readwrite("gaus_amp", &SPEChargeDistribution::gaus_amp)
    .def_readwrite("gaus_mean", &SPEChargeDistribution::gaus_mean)
    .def_readwrite("gaus_width", &SPEChargeDistribution::gaus_width)
    .add_property("is_valid", &SPEChargeDistribution::IsValid)
    .def(dataclass_suite<SPEChargeDistribution>())
    ;
}
