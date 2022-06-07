/**
 *  $Id$
 *  
 *  Copyright (C) 2004, 2005, 2006, 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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

void do_no_harm(const I3Tray& tray){}

void Execute_0(I3Tray &tray) {
  try {
    tray.Execute();
  }
  catch(const sigint_exception& x)
  {
    PyErr_SetString(PyExc_KeyboardInterrupt, x.what());
    throw_error_already_set();
  }
}

void Execute_1(I3Tray &tray, unsigned n) {
  try {
    tray.Execute(n);
  }
  catch(const sigint_exception& x)
  {
    PyErr_SetString(PyExc_KeyboardInterrupt, x.what());
    throw_error_already_set();
  }
}


void register_I3Tray()
{

  class_<I3Tray::param_setter>("param_setter", 
			       init<const I3Tray::param_setter&>());

  //void (I3Tray::*Execute_0)(void)              = &I3Tray::Execute;
  //void (I3Tray::*Execute_1)(unsigned)          = &I3Tray::Execute;

  class_<I3Tray, boost::noncopyable>("I3Tray")
    .def("Execute", Execute_0)
    .def("Execute", Execute_1)
    .def("Usage", &I3Tray::Usage)
    .def("Finish", do_no_harm)
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


