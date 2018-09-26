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

#include <vector>

#include <dataclasses/physics/I3FlasherInfo.h>
#include "dataclasses/I3Vector.h"
#include <dataclasses/physics/I3DOMLaunch.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

namespace {

bool equivalent(double a, double b)
{
  return a == b || (std::isnan(a) && std::isnan(b));
}

bool operator==(const std::vector<int>&a, const std::vector<int>&b)
{
  if (a.size() != b.size())
    return false;
  typedef std::vector<int>::const_iterator iter;
  for(iter ia=a.begin(), ib=b.begin(); ia!=a.end(); ia++,ib++)
    if (*ia != *ib)
      return false;
  return true;
}

}

bool operator==(const I3FlasherInfo &a, const I3FlasherInfo &b)
{
  return a.GetFlashingOM() == b.GetFlashingOM()
    && equivalent(a.GetFlashTime(), b.GetFlashTime())
    && equivalent(a.GetATWDBinSize(), b.GetATWDBinSize())
    && a.GetLEDBrightness() == b.GetLEDBrightness()
    && a.GetMask() == b.GetMask()
    && a.GetWidth() == b.GetWidth()
    && a.GetRate() == b.GetRate()
    && a.GetRawATWD3() == b.GetRawATWD3();
}

bool equal_flasherinfo(const I3FlasherInfo fi1, const I3FlasherInfo fi2){
  return fi1==fi2;
}


void register_I3FlasherInfo()
{
 const std::vector<int>& (I3FlasherInfo::*get_waveform)() const = &I3FlasherInfo::GetRawATWD3;
 {
   scope outer = 
   class_<I3FlasherInfo, bases<I3FrameObject>, I3FlasherInfoPtr>("I3FlasherInfo")
     #define PROPS (FlashingOM)(FlashTime)(ATWDBinSize)(LEDBrightness)(Mask)(Width)(Rate)
     BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3FlasherInfo, PROPS)
     #undef PROPS
     .add_property("raw_atwd3", make_function(get_waveform,return_internal_reference<1>()), &I3FlasherInfo::SetRawATWD3)
     .def(dataclass_suite<I3FlasherInfo>())
     .def("__eq__", equal_flasherinfo)
   ;

 }

 class_<I3FlasherInfoVect, bases<I3FrameObject>, I3FlasherInfoVectPtr>("I3FlasherInfoVect")
   .def(dataclass_suite<I3FlasherInfoVect>())
   ;

 register_pointer_conversions<I3FlasherInfo>();
 register_pointer_conversions<I3FlasherInfoVect>();
}
