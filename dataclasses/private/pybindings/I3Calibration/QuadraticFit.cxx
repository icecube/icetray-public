//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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

void register_QuadraticFit()
{
  class_<QuadraticFit>("QuadraticFit")
    .def_readwrite("quad_fit_a", &QuadraticFit::quadFitA)
    .def_readwrite("quad_fit_b", &QuadraticFit::quadFitB)
    .def_readwrite("quad_fit_c", &QuadraticFit::quadFitC)
    .def(dataclass_suite<QuadraticFit>())
    ;
}
