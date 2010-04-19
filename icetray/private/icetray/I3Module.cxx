/**
 *  $Id: I3Module.cxx 51710 2009-01-06 19:23:39Z troy $
 *  
 *  Copyright (C) 2004 - 2008
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Simon Patton, John Pretz, and the IceCube Collaboration <http://www.icecube.wisc.edu>
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
#include <sys/time.h>
#include <sys/resource.h>

#include <boost/bind.hpp>
#include <boost/preprocessor.hpp>
#include <boost/foreach.hpp>

#include <icetray/I3Module.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Context.h>
#include <icetray/I3Tray.h>
#include <icetray/I3PhysicsUsage.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3Frame.h>
#include <icetray/impl.h>

const double I3Module::min_report_time_ = 10;

class ModuleTimer
{
  double& sys;
  double& user;
  struct rusage stop, start;
   bool fail;
 public:
   ModuleTimer(double& s, double& u) : sys(s), user(u)
  {
    fail = (getrusage(RUSAGE_SELF, &start) == -1);
  }
  ~ModuleTimer()
  {
    if (getrusage(RUSAGE_SELF, &stop) != -1 && !fail)
      {
	user += (stop.ru_utime.tv_sec - start.ru_utime.tv_sec);
	user += double(stop.ru_utime.tv_usec - start.ru_utime.tv_usec) / 10E+06;

	sys += (stop.ru_stime.tv_sec - start.ru_stime.tv_sec);
	sys += double(stop.ru_stime.tv_usec - start.ru_stime.tv_usec) / 10E+06;
      }
  }
};

I3Module::I3Module(const I3Context& context) 
  : context_(context),
    outboxes_(context_.Get<outboxmap_t>("OutBoxes")),
    configuration_(context_.Get<I3Configuration>())
{
  const std::string& instancename = context_.Get<I3Configuration>().InstanceName();
  SetName(instancename);
  ncall_ = 0;
  systime_ = usertime_ = 0;
  i3_log("%s done", __PRETTY_FUNCTION__);
}

I3Module::~I3Module()
{ 
  // only print if more than 10 seconds used.  This is kind of an
  // arbitrary number.
  if (usertime_ + systime_ > min_report_time_)
    printf("%40s: %6u calls to physics %9.2fs user %9.2fs system\n",
	   GetName().c_str(), ncall_, usertime_, systime_);
}

void 
I3Module::Flush()
{
  for (outboxmap_t::iterator iter = outboxes_.begin();
       iter != outboxes_.end();
       iter++)
    {
      I3ModulePtr nextmodule = iter->second.second;
      FrameFifoPtr fifo = iter->second.first;

      if (nextmodule)
	{
	  while (fifo->size())
	    {
	      nextmodule->Do(&I3Module::Process_);
	    }
	}
    }
}

void 
I3Module::Do(void (I3Module::*f)())
{
#ifndef I3_JEB
  I3Tray::SetActiveContext(&context_);
#endif

  (this->*f)();

#ifndef I3_JEB
  I3Tray::SetActiveContext(0);
#endif

  for (outboxmap_t::iterator iter = outboxes_.begin();
       iter != outboxes_.end();
       iter++)
    {
      I3ModulePtr nextmodule = iter->second.second;
      FrameFifoPtr fifo = iter->second.first;

      log_trace("%s: %zu frames in fifo %s", 
		GetName().c_str(), fifo->size(), iter->first.c_str()); 

      if (nextmodule)
	{
	  if (f == &I3Module::Process_)
	    {
	      while (fifo->size())
		nextmodule->Do(f);
	    }
	  else
	    nextmodule->Do(f);
	}
    }
}

void
I3Module::Configure_()
{
  this->Configure();
  inbox_ = context_.Get<FrameFifoPtr>("InBox");

  //
  // HACK HACK HACK
  //
  // until we overhaul the way icetray sets up modules, this is a
  // hack.  ConnectBoxes is called by the framework, which creates
  // slots in the outboxes_ structure without reference to whether we
  // AddOutBox them or not.  So after Configure, delete any that
  // shouldn't have been created.
  //
  vector<string> bad_boxes;
  for (outboxmap_t::iterator iter = outboxes_.begin();
       iter != outboxes_.end();
       iter++)
    {
      set<string>::iterator boxcheck = added_boxes.find(iter->first);
      if (boxcheck == added_boxes.end())
	bad_boxes.push_back(iter->first);
    }
  BOOST_FOREACH(const string& box, bad_boxes)
    {
      outboxes_.erase(box);
    }
}

void
I3Module::Configure()
{

}

void
I3Module::Register(const I3Frame::Stream& when, boost::function<void(I3FramePtr)> what)
{
  methods_[when] = what;
}

void
I3Module::Process_()
{
  i3_log("%s", __PRETTY_FUNCTION__);

  if (inbox_)
    log_trace("%s: %zu frames in inbox", GetName().c_str(), inbox_->size());

  I3FramePtr frame = PeekFrame();

  log_trace("frame=%p", frame.get());
  if (!frame)
    {
      log_trace("no frame, calling this->Process() in case we're a driving module");
      this->Process();
      return;
    }

  if (ShouldDoProcess(frame))
    this->Process();
  else
    {
      PopFrame();
      PushFrame(frame);
    }
}

void
I3Module::Process()
{
  i3_log("%s", __PRETTY_FUNCTION__);

  if (inbox_)
    log_trace("%zu frames in inbox", inbox_->size());

  I3FramePtr frame = PopFrame();

  if (!frame)
    return;

  methods_t::iterator miter = methods_.find(frame->GetStop());

  if (miter != methods_.end())
    {
      miter->second(frame);
      return;
    }

  if(frame->GetStop() == I3Frame::Physics && ShouldDoPhysics(frame))
    {
      ModuleTimer mt(systime_, usertime_);
      ++ncall_;
      Physics(frame);
    }
  else if(frame->GetStop() == I3Frame::Geometry && ShouldDoGeometry(frame))
    Geometry(frame);
  else if(frame->GetStop() == I3Frame::Calibration && ShouldDoCalibration(frame))
    Calibration(frame);
  else if(frame->GetStop() == I3Frame::DetectorStatus && ShouldDoDetectorStatus(frame))
    DetectorStatus(frame);
  else if(ShouldDoOtherStops(frame))
    OtherStops(frame);
}

void
I3Module::AddOutBox(const std::string& s)
{
  // we *could* check for double-adds of outboxes here, but this isn't likely
  // to catch very many buggy situations, and it breaks lots of code, for instance
  // if you switch from I3Module to I3ConditionalModule, you'd have to remove your
  // AddOutBox()

  // keep a list of outboxes that the user actually adds, for 
  // sanitychecking in Configure_()
  added_boxes.insert(s);

  // should check that the ConnectBoxes boxes match
  if (!outboxes_.count(s))
    log_fatal("Module \"%s\" attempting to add outbox \"%s\" which isn't connected to anything.\nI3Trays need to be fully connected... do you need to add a TrashCan at the end?",
	      GetName().c_str(), s.c_str());
}

void
I3Module::AddParameter(const std::string& name, const std::string& description)
{
  configuration_.Add(name, description);
}

I3FramePtr
I3Module::PopFrame()
{
  if (!inbox_)
    return I3FramePtr();

  if (inbox_->begin() == inbox_->end())
    return I3FramePtr();

  I3FramePtr frame = inbox_->back();
  inbox_->pop_back();
  return frame;
}

void
I3Module::PushFrame(I3FramePtr frameptr, const string& name)
{
  outboxmap_t::iterator iter = outboxes_.find(name);

  if (iter == outboxes_.end())
    log_fatal("Module \"%s\" attempted to push a frame onto an outbox name \"%s\" which either doesn't exist or isn't connected to anything.  Check steering file.",
	      GetName().c_str(), name.c_str());

  iter->second.first->push_front(frameptr);

  log_trace("%s pushed frame onto fifo \"%s\"", GetName().c_str(), name.c_str());
}

void
I3Module::PushFrame(I3FramePtr frameptr)
{
  if (outboxes_.begin() == outboxes_.end())
    log_fatal("Module \"%s\" of type \"%s\" wants to PushFrame, but created no Outboxes",
	      GetName().c_str(), I3::name_of(typeid(*this)).c_str());

  for (outboxmap_t::iterator iter = outboxes_.begin();
       iter != outboxes_.end();
       iter++)
    {
      iter->second.first->push_front(frameptr);
      log_trace("%s pushed frame onto fifo \"%s\"", GetName().c_str(), iter->first.c_str());
    }
}

void
I3Module::Suspend()
{
}

void
I3Module::Reconfigure()
{
}

void
I3Module::Resume()
{
}

void
I3Module::Finish()
{
  i3_log("%s", __PRETTY_FUNCTION__);
}

void
I3Module::Dispose()
{
  i3_log("%s", __PRETTY_FUNCTION__);
}

void
I3Module::Abort()
{
  i3_log("%s", __PRETTY_FUNCTION__);
}

// Stop routines

void
I3Module::Physics(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoPhysics(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void
I3Module::Monitoring(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoMonitoring(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void
I3Module::TimeCal(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoTimeCal(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void 
I3Module::Geometry(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoGeometry(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void 
I3Module::Calibration(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoCalibration(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void
I3Module::DetectorStatus(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoDetectorStatus(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void
I3Module::OtherStops(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  PushFrame(frame);
}

bool
I3Module::ShouldDoOtherStops(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

void
I3Module::RequestSuspension() const
{
  i3_log("%s", __PRETTY_FUNCTION__);
  I3Tray::RequestSuspension();
}


I3PhysicsUsage
I3Module::ReportUsage()
{
  I3PhysicsUsage pu;
  pu.systime = systime_;
  pu.usertime = usertime_;
  pu.ncall = ncall_;
  return pu;
}

I3FramePtr
I3Module::PeekFrame()
{
  if (!inbox_)
    return I3FramePtr();
  if (inbox_->begin() == inbox_->end())
    return I3FramePtr();
  return inbox_->back();
}

bool
I3Module::ShouldDoProcess(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  return true;
}

