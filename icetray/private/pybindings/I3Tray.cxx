/**
 *  $Id: ithon.cxx 43705 2008-03-26 20:54:18Z kjmeagher $
 *  
 *  Copyright (C) 2004, 2005, 2006, 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */


#include <boost/python.hpp>
#include <boost/preprocessor.hpp>

#include <string>
#include <iostream>
#include <iomanip>
#include <signal.h>

#include <icetray/I3Logging.h>
#include <icetray/I3Tray.h>
#include <icetray/OMKey.h>


// AddService: same as AddModule
I3Tray::param_setter (I3Tray::*AddServiceV)(const std::string& classname, 
					    const std::string& instancename)
  = &I3Tray::AddService;

//
// Manual enumeration of overloads for parameters.  For each type in
// I3_PARAM_TYPES a pointer to a member function with an appropriate
// signature is generated.  Each of these must be individually exposed
// to python via "def()"
//
typedef std::vector<int> ithon_vector_int;
typedef std::vector<double> ithon_vector_double;
typedef std::vector<std::string> ithon_vector_string;
typedef std::vector<OMKey> ithon_vector_omkey;

#define ITHON_I3_PARAM_TYPES			\
  (ithon_vector_int)				\
    (ithon_vector_double)			\
    (ithon_vector_string)			\
    (ithon_vector_omkey)			\
    (string)					\
    (double)					\
    (int)					\
    (int64_t)					\
    (OMKey)


#define I3_SETPARAM_OVERLOAD(r,data,t) \
  bool (I3Tray::*BOOST_PP_CAT(sp_,t) )(const std::string& something, const std::string& somethingelse, const t&) = &I3Tray::SetParameter;

BOOST_PP_SEQ_FOR_EACH(I3_SETPARAM_OVERLOAD, ~, ITHON_I3_PARAM_TYPES);

// this is used in the interface definition to expose these function
// to python
#define I3_SETPARAM_OVERLOAD_DEF(r,data,t) .def("SetParameter", BOOST_PP_CAT(sp_,t)) 


//
// Main python interface definition.  The name "ithon" must match
// the filename of the library that it is loaded from.  A C-linkage
// function named initithon() is generated and put in the library.
// When python loads a file name libX.so it calls a function named
// initX().  If it can't find it, it bails out.
//
using namespace boost::python;

void register_I3Tray()
{

  class_<I3Tray::param_setter>("param_setter", 
			       init<const I3Tray::param_setter&>());

  void (I3Tray::*Execute_0)(void)              = &I3Tray::Execute;
  void (I3Tray::*Execute_1)(unsigned)          = &I3Tray::Execute;

  class_<I3Tray, boost::noncopyable>("I3Tray")
    .def("Execute", Execute_0)
    .def("Execute", Execute_1)
    .def("Usage", &I3Tray::Usage)
    .def("Finish", &I3Tray::Finish)
    .def("AddService", 
	 (I3Tray::param_setter (I3Tray::*)(const std::string&, const std::string&))
	 &I3Tray::AddService)
    .def("AddModule", 
	 (I3Tray::param_setter (I3Tray::*)(boost::python::object, const std::string&))
	 &I3Tray::AddModule)
    .def("ConnectBoxes", &I3Tray::ConnectBoxes)
    
    // SetParameter exposure: BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS
    // does not work for some reason...  Compiler can't determine the
    // type of &I3Tray::SetParameter.  Dunno why.
    // .def("SetParameter", &I3Tray::SetParameter, SetParameterOverloads()) 

    // instead we do it manually with a little preprocessor
    // metaprogramming, and five-point-palm exploding heart
    // technique...
    .def("SetParameter", (bool (I3Tray::*)(const std::string&,
					   const std::string&,
					   const boost::python::object&))&I3Tray::SetParameter)
	 
    // BOOST_PP_SEQ_FOR_EACH(I3_SETPARAM_OVERLOAD_DEF, ~, ITHON_I3_PARAM_TYPES)
    ;
  
}


