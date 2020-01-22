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

#include <dataclasses/physics/I3Waveform.h>

#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

std::string to_str(const I3Waveform theWaveform){
    std::ostringstream oss;
    oss << theWaveform << std::flush;
    return oss.str();
}

void register_I3Waveform()
{
  std::vector<double>& (I3Waveform::*get_waveform)() = &I3Waveform::GetWaveform;
  const std::vector<I3Waveform::StatusCompound>& 
    (I3Waveform::*get_waveform_information)() const = &I3Waveform::GetWaveformInformation;
  object get_waveform_func = make_function(get_waveform, return_internal_reference<1>());
  object get_waveform_information_func = make_function(get_waveform_information, return_internal_reference<1>());
  unsigned (*get_status_static)(const std::vector<I3Waveform::StatusCompound>&) = &I3Waveform::GetStatus;
  unsigned (I3Waveform::*get_status_member)() const = &I3Waveform::GetStatus;

  {
    scope waveform_scope =
      class_<I3Waveform, boost::shared_ptr<I3Waveform> >("I3Waveform")
      .def(copy_suite<I3Waveform>())
      #define PROPS (BinWidth)(SourceIndex)(Digitizer)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Waveform, PROPS)
      #undef PROPS
      #define PROPS (Channel)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Waveform, PROPS)
      #undef PROPS
      .add_property(snake_case("Source"), &I3Waveform::GetSource)
      .add_property("status", get_status_member)
      .add_property("time", &I3Waveform::GetStartTime, &I3Waveform::SetStartTime)
      .add_property("binwidth", &I3Waveform::GetBinWidth, &I3Waveform::SetBinWidth)
      .add_property("waveform", get_waveform_func, &I3Waveform::SetWaveform)
      .add_property("waveform_information", get_waveform_information_func, &I3Waveform::SetWaveformInformation)
      .add_property("hlc", &I3Waveform::IsHLC, &I3Waveform::SetHLC)

      // for static methods you need the both of these
      .def("get_status", get_status_static)
      .staticmethod("get_status")
      .def(self == self)
      .def(dataclass_suite<I3Waveform>())
      .def("__str__", to_str)
     ;

    const std::pair<unsigned long long int, unsigned long long int>&
      (I3Waveform::StatusCompound::* get_interval)() const 
      = &I3Waveform::StatusCompound::GetInterval;

    class_<I3Waveform::StatusCompound>("StatusCompound")
      #define PROPS (Status)(Channel)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Waveform::StatusCompound, PROPS)
      #undef PROPS
      .add_property("interval", make_function(get_interval, return_value_policy<copy_const_reference>()))
      .def( freeze() )
      ;

    enum_<I3Waveform::Source>("Source")
      .value("ATWD", I3Waveform::ATWD)
      .value("FADC", I3Waveform::FADC)
      .value("TWR_ELECTRICAL", I3Waveform::TWR_ELECTRICAL)
      .value("TWR_OPTICAL", I3Waveform::TWR_OPTICAL)
      .value("ETC", I3Waveform::ETC)
      .value("SLC", I3Waveform::SLC)
      .export_values()
      ;

    enum_<I3Waveform::Status>("Status")
      .value("VIRGINAL", I3Waveform::VIRGINAL)
      .value("COMBINED", I3Waveform::COMBINED)
      .value("SATURATED", I3Waveform::SATURATED)
      .value("UNDERSHOT", I3Waveform::UNDERSHOT)
      .export_values()
      ;
  }

}
