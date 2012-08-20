//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: ithon.cxx 25598 2006-11-25 02:52:57Z troy $
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
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3PacketModule.h>
#include <icetray/I3Logging.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/OMKey.h>
#include <icetray/PythonModule.h>
#include <icetray/impl.h>
#include <boost/python/import.hpp>

namespace bp = boost::python;
template <typename Base>
PythonModule<Base>::PythonModule(const I3Context& ctx) : Base(ctx)
{ 
  i3_log("%s", __PRETTY_FUNCTION__);
}

template <>
PythonModule<I3PacketModule>::PythonModule(const I3Context& ctx) :
  I3PacketModule(ctx, I3Frame::DAQ)
{ 
  i3_log("%s", __PRETTY_FUNCTION__);
}

template <>
PythonModule<I3PacketModule>::PythonModule(const I3Context& ctx,
  I3Frame::Stream stream) : I3PacketModule(ctx, stream)
{ 
  i3_log("%s", __PRETTY_FUNCTION__);
}

template <typename Base>
void 
PythonModule<Base>::PyConfigure()
{
  Base::Configure();
}

template <typename Base>
void 
PythonModule<Base>::Configure()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override conf = this->get_override("Configure"))
    conf();
  else
    log_fatal("Python module doesn't implement 'Configure'");
}

template <typename Base>
void 
PythonModule<Base>::PyProcess()
{
  Base::Process();
}

template <typename Base>
void 
PythonModule<Base>::Process()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override process = this->get_override("Process"))
    {
      log_trace("%s", "calling override for Process");
      process();
    }
  else
    {
      log_trace("%s", "calling base Process");
      Base::Process();
    }
}

template <typename Base>
void 
PythonModule<Base>::PyFinish()
{
  Base::Finish();
}

template <typename Base>
void 
PythonModule<Base>::Finish()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override fin = this->get_override("Finish"))
    fin();
  else
    Base::Finish();
}

template <typename Base>
void
PythonModule<Base>::AddParameter(const std::string& name, 
			   const std::string& description, 
			   const bp::object& value)
{
  Base::AddParameter(name, description, value);
}

template <typename Base>
void
PythonModule<Base>::AddParameter(const std::string& name, 
			   const std::string& description) 
{
  Base::AddParameter(name, description);
}

template <typename Base>
void
PythonModule<Base>::Register(const I3Frame::Stream& s, const boost::python::object& method)
{
  Base::Register(s, method);
}

template <typename Base>
bp::object
PythonModule<Base>::GetParameter(const std::string& name)
{
  bp::object obj;
  Base::GetParameter(name, obj);
  return obj;
}
  
template <typename Base>
bool
PythonModule<Base>::ShouldDoGeometry(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override sd = this->get_override("ShouldDoGeometry"))
    return sd(frame);
  else
    return Base::ShouldDoGeometry(frame);
}

template <typename Base>
void 
PythonModule<Base>::Geometry(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override g = this->get_override("Geometry"))
    g(frame);
  else
    Base::Geometry(frame);
}

template <typename Base>
bool
PythonModule<Base>::ShouldDoCalibration(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override sd = this->get_override("ShouldDoCalibration"))
    return sd(frame);
  else
    return Base::ShouldDoCalibration(frame);
}

template <typename Base>
void 
PythonModule<Base>::Calibration(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override cal = this->get_override("Calibration"))
    cal(frame);
  else
    Base::Calibration(frame);
}

template <typename Base>
bool
PythonModule<Base>::ShouldDoDAQ(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override sd = this->get_override("ShouldDoDAQ"))
    return sd(frame);
  else
    return Base::ShouldDoDAQ(frame);
}

template <typename Base>
void 
PythonModule<Base>::DAQ(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override ds = this->get_override("DAQ"))
    ds(frame);
  else
    Base::DAQ(frame);
}

template <typename Base>
bool
PythonModule<Base>::ShouldDoDetectorStatus(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override sd = this->get_override("ShouldDoDetectorStatus"))
    return sd(frame);
  else
    return Base::ShouldDoDetectorStatus(frame);
}

template <typename Base>
void 
PythonModule<Base>::DetectorStatus(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override ds = this->get_override("DetectorStatus"))
    ds(frame);
  else
    Base::DetectorStatus(frame);
}

template <typename Base>
bool
PythonModule<Base>::ShouldDoPhysics(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override sd = this->get_override("ShouldDoPhysics"))
    return sd(frame);
  else
    return Base::ShouldDoPhysics(frame);
}

template <typename Base>
void 
PythonModule<Base>::Physics(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override phys = this->get_override("Physics"))
    phys(frame);
  else
    Base::Physics(frame);
}

template <typename Base>
I3FramePtr
PythonModule<Base>::PopFrame()
{
  return Base::PopFrame();
}

template <typename Base>
void
PythonModule<Base>::PushFrame(I3FramePtr f)
{
  return Base::PushFrame(f);
}

template <typename Base>
void
PythonModule<Base>::PushFrame(I3FramePtr f, const std::string& where)
{
  return Base::PushFrame(f, where);
}

template <typename Base>
void
PythonModule<Base>::RequestSuspension()
{
  return Base::RequestSuspension();
}

template <typename Base>
void
PythonModule<Base>::AddOutBox(const std::string& name)
{
  Base::AddOutBox(name);
}

template <>
void 
PythonModule<I3PacketModule>::FramePacket(std::vector<I3FramePtr> &frames)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  if (bp::override pkt = this->get_override("FramePacket"))
    pkt(frames);
  else
    I3PacketModule::FramePacket(frames);
}

template <>
I3Frame::Stream
PythonModule<I3PacketModule>::GetSentinel()
{ return sentinel; }

template <>
void
PythonModule<I3PacketModule>::SetSentinel(I3Frame::Stream sent)
{ sentinel = sent; }

template <>
const std::vector<I3Frame::Stream> &
PythonModule<I3PacketModule>::GetPacketTypes()
{ return packet_types; }

template <>
void PythonModule<I3PacketModule>::SetPacketTypes(
    const std::vector<I3Frame::Stream> &types)
{ packet_types = types; }

template class PythonModule<I3Module>;
template class PythonModule<I3ConditionalModule>;
template class PythonModule<I3PacketModule>;
