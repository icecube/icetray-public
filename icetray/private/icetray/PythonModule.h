//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: PythonModule.h 165886 2018-10-01 14:37:58Z nwhitehorn $
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

#include <icetray/I3Module.h>
#include <icetray/I3Logging.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/OMKey.h>
#include <boost/python.hpp>


struct PythonModule : I3Module, boost::python::wrapper<I3Module>
{
  PythonModule(const I3Context& ctx); 

  void Configure();

  void Process();

  void Finish();

  void AddParameter(const std::string& name, 
		    const std::string& description, 
		    const boost::python::object& value);

  void AddParameter(const std::string& name, 
		    const std::string& description); 

  boost::python::object GetParameter(const std::string& name);
  
  void Register(const I3Frame::Stream&, const boost::python::object& method);

  void Geometry(I3FramePtr frame);
  void Calibration(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);
  void Physics(I3FramePtr frame);


  void PushFrame(I3FramePtr frame);
  void PushFrame(I3FramePtr frame, const std::string& where);

  void AddOutBox(const std::string& name);
  I3FramePtr PopFrame();

};


