/**
 *  $Id$
 *
 *  Copyright (C) 2008
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

#include <icetray/I3Logging.h>
#include <icetray/load_project.h>
#include <icetray/scratch.h>
#include <signal.h>

void register_OMKey();
void register_I3Bool();
void register_I3Int();
void register_I3PhysicsUsage();
void register_I3RUsage();
void register_I3TrayInfo();
void register_I3Tray();
void register_I3Units();
void register_I3Frame();
void register_I3FrameObject();
void register_I3FrameMixing();
void register_I3Logging();
void register_I3Module();
void register_I3Context();
void register_I3Configuration();
void register_I3ModuleFactory();
void register_MyService();

void register_std_cont_pod_char();
void register_std_cont_pod_double();
void register_std_cont_pod_I3Frame_Stream();
void register_std_cont_pod_int();
void register_std_cont_pod_map_int_int();
void register_std_cont_pod_map_omkey_int();
void register_std_cont_pod_omkey();
void register_std_cont_pod_pairs();
void register_std_cont_pod_short();
void register_std_cont_pod_string();
void register_std_cont_pod_ulong();
void register_std_cont_pod_unsigned();
void register_std_cont_pod_vector_int();
void register_std_cont_pod_vector_string();

using namespace boost::python;

void load_impl(const std::string& name, bool verbose = true ){
  load_project(name,verbose);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(load_impl_overloads, load_impl,1,2);

I3_PYTHON_MODULE(icetray)
{
  // 
  // You *must* do this first, or you get the nasty error 
  //   void icecube::serialization::detail::tkmap::insert(const
  //   icecube::serialization::extended_type_info*): Assertion `lookup(eti) ==  
  //   slf->m_map.tend()' failed. 
  // 
  load_project("icetray", false); 

  boost::python::scope().attr("I3Default") = boost::python::eval("object")();
  def("load", &load_impl, load_impl_overloads(args("name","verbose")));
  def("get_scratch_directory", &I3::dataio::GetScratchDirectory);
  def("set_scratch_directory", &I3::dataio::SetScratchDirectory);

  register_OMKey();
  register_I3PhysicsUsage();
  register_I3Tray();
  register_I3Units();
  register_I3Frame();
  register_I3FrameObject();
  register_I3FrameMixing();
  register_I3Logging();
  register_I3Module();
  register_I3Context();
  register_I3Configuration();
  register_I3ModuleFactory();
  register_I3RUsage();
  register_I3TrayInfo();
  register_I3Bool();
  register_I3Int();
  register_MyService();

  register_std_cont_pod_char();
  register_std_cont_pod_double();
  register_std_cont_pod_I3Frame_Stream();
  register_std_cont_pod_int();
  register_std_cont_pod_map_int_int();
  register_std_cont_pod_map_omkey_int();
  register_std_cont_pod_omkey();
  register_std_cont_pod_pairs();
  register_std_cont_pod_short();
  register_std_cont_pod_string();
  register_std_cont_pod_ulong();
  register_std_cont_pod_unsigned();
  register_std_cont_pod_vector_int();
  register_std_cont_pod_vector_string();
}
