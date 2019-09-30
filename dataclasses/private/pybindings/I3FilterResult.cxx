//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
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
