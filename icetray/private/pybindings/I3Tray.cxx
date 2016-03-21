/**
 *  $Id$
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

using std::string;

//
// Main python interface definition.
//
using namespace boost::python;

static std::string I3TrayString(I3Tray &tray) {
  std::stringstream str;
  str << tray;
  return str.str();
}

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
    .def("RequestSuspension", &I3Tray::RequestSuspension)
    .def("TrayInfo", &I3Tray::TrayInfo)
    .def("__str__", &I3TrayString)
    .add_property("tray_info", &I3Tray::TrayInfo)
    .add_property("context", make_function(&I3Tray::GetContext, return_internal_reference<>()))
    .def("AddService", 
	 (I3Tray::param_setter (I3Tray::*)(const std::string&, std::string))
	 &I3Tray::AddService)
    .def("AddModule", 
	 (I3Tray::param_setter (I3Tray::*)(boost::python::object, std::string))
	 &I3Tray::AddModule)
    .def("MoveModule", &I3Tray::MoveModule, (arg("self"), arg("name"), arg("anchor"), arg("after")=false))
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
    ;
  
}


