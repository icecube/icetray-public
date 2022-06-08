#ifndef ICETRAY_PYTHONOMODULE_H_INCLUDED
#define ICETRAY_PYTHONOMODULE_H_INCLUDED
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

#include <icetray/I3Module.h>
#include <icetray/I3Logging.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/OMKey.h>
#include <boost/python.hpp>


template <typename Base>
struct PythonModule : Base, boost::python::wrapper<Base>
{
  PythonModule(const I3Context& ctx); 
  PythonModule(const I3Context& ctx, I3Frame::Stream); 

  void PyConfigure();
  void Configure();

  void PyProcess();
  void Process();

  void PyFinish();
  void Finish();

  void AddParameter(const std::string& name, 
		    const std::string& description, 
		    const boost::python::object& value);

  void AddParameter(const std::string& name, 
		    const std::string& description); 

  boost::python::object GetParameter(const std::string& name);
  
  void Register(const I3Frame::Stream&, const boost::python::object& method);

  bool ShouldDoGeometry(I3FramePtr frame);
  void Geometry(I3FramePtr frame);

  bool ShouldDoCalibration(I3FramePtr frame);
  void Calibration(I3FramePtr frame);

  bool ShouldDoDetectorStatus(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);

  bool ShouldDoSimulation(I3FramePtr frame);
  void Simulation(I3FramePtr frame);

  bool ShouldDoDAQ(I3FramePtr frame);
  void DAQ(I3FramePtr frame);

  bool ShouldDoPhysics(I3FramePtr frame);
  void Physics(I3FramePtr frame);


  void PushFrame(I3FramePtr frame);
  void PushFrame(I3FramePtr frame, const std::string& where);

  void RequestSuspension();

  void AddOutBox(const std::string& name);
  I3FramePtr PopFrame();

  const I3Context& GetContext() const { return Base::context_; }
  const I3Configuration& GetConfiguration() const { return Base::configuration_; }

  // Just for I3PacketModule wrapper
  void FramePacket(std::vector<I3FramePtr> &);
  I3Frame::Stream GetSentinel();
  void SetSentinel(I3Frame::Stream);
  const std::vector<I3Frame::Stream> &GetPacketTypes();
  void SetPacketTypes(const std::vector<I3Frame::Stream> &);

  SET_LOGGER("PythonModule");
};


#endif
