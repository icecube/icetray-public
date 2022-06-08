//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
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

using namespace boost::python;
namespace bp = boost::python;

#include <dataclasses/physics/I3FilterResult.h>
#include <boost/foreach.hpp>
#include <icetray/python/std_map_indexing_suite.hpp>
#include <icetray/python/dataclass_suite.hpp>

static std::string 
I3FilterResultMap_prettyprint(const I3FilterResultMap& filter_result_map)
{
  std::ostringstream oss;
  oss << "{ \n" ;
  BOOST_FOREACH(I3FilterResultMap::const_reference filter_result,filter_result_map)
    {
      oss << '\t' 
	  << filter_result.first << " : " 
	  << filter_result.second.conditionPassed << " , "
	  << filter_result.second.prescalePassed << "\n"
	;
    }

  oss << " }\n";
  return oss.str();
}

static std::string 
I3FilterResult_prettyprint(const I3FilterResult& filter_result)
{
  std::ostringstream oss;
  oss << "[ I3FilterResult : " 
      << filter_result.conditionPassed << " , "
      << filter_result.prescalePassed << " ]"
    ;
  return oss.str();
}

bool
I3FilterResult_bool(const I3FilterResult& filter_result)
{
  return filter_result.conditionPassed && filter_result.prescalePassed;
}

void register_I3FilterResult()
{
  class_<I3FilterResult, bases<I3FrameObject>, boost::shared_ptr<I3FilterResult> >("I3FilterResult")
    .def_readwrite("condition_passed", &I3FilterResult::conditionPassed)
    .def_readwrite("prescale_passed" , &I3FilterResult::prescalePassed)
    .def("__str__",I3FilterResult_prettyprint)
    .def("__nonzero__",I3FilterResult_bool)
    .def("__bool__",I3FilterResult_bool)    
    .def(operator_suite<I3FilterResult>())
    .def( freeze() )
    ;
  register_pointer_conversions<I3FilterResult>();

  class_<I3FilterResultMap, bases<I3FrameObject>, I3FilterResultMapPtr>("I3FilterResultMap")
    .def("__str__",I3FilterResultMap_prettyprint)
    .def(std_map_indexing_suite<I3FilterResultMap>())
    .def(operator_suite<I3FilterResultMap>())
    ;
  register_pointer_conversions<I3FilterResultMap>();
  
}
