/**
 *  $Id$
 *  
 *  Copyright (C) 2008
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

#include <icetray/I3Logging.h>
#include <icetray/load_project.h>
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

BOOST_PYTHON_MODULE(icetray)
{
  // 
  // You *must* do this first, or you get the nasty error 
  //   void boost::serialization::detail::tkmap::insert(const   
  //   boost::serialization::extended_type_info*): Assertion `lookup(eti) ==  
  //   slf->m_map.tend()' failed. 
  // 
  load_project("icetray", false); 

  def("load", &load_impl, load_impl_overloads(args("name","verbose")));

#ifdef I3_USE_ROOT
  // undo that nasty irritating root signal catching.  Power to the
  // ctrl-C.
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGTSTP, SIG_DFL);
  signal(SIGHUP, SIG_DFL);
  signal(SIGCHLD, SIG_DFL);
#endif

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
