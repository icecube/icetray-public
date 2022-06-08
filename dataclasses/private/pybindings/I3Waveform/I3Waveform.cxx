//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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
      .value("SiPM", I3Waveform::SiPM)
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
