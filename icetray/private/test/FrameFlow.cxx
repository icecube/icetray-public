/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/Utility.h>

#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

TEST_GROUP(FrameFlow);

struct FFSource : public I3Module
{
  unsigned nextstop;

  FFSource(const I3Context& context) : I3Module(context) 
  { 
    nextstop = 1;
  }

  void Process() 
  {
    I3Frame::Stream stop;
    switch(nextstop)
      {
      case 1:
	stop = I3Frame::Geometry;
	break;
      case 2:
	stop = I3Frame::Calibration;
	break;
      case 3:
	stop = I3Frame::DetectorStatus;
	break;
      case 4:
	stop = I3Frame::Physics;
	break;
      }

    I3FramePtr frame(new I3Frame(stop));
    PushFrame(frame);
    nextstop = (nextstop % 4) + 1;
  }
};

I3_MODULE(FFSource);

struct Nothing : public I3Module
{
  Nothing(const I3Context& context) : I3Module(context)
  {}
  
  void Physics(I3FramePtr frame)
  {
    PushFrame(frame);
  }
};

I3_MODULE(Nothing);


std::map<std::string, std::map<I3Frame::Stream, unsigned> > counts;

struct FFCounter : public I3Module
{
  FFCounter(const I3Context& context) : I3Module(context)  
 {}
  
#define IMPLEMENT(STOP)							\
  void STOP(I3FramePtr frame) {						\
    ENSURE_EQUAL(frame->GetStop(), I3Frame::STOP);			\
    counts[GetName()][I3Frame::STOP]++;					\
  }

  IMPLEMENT(Geometry)
  IMPLEMENT(Calibration)
  IMPLEMENT(DetectorStatus)
  IMPLEMENT(Physics)
  
#undef IMPLEMENT
};

I3_MODULE(FFCounter);

TEST(straight_line)
{
  I3Tray tray;
  counts.clear();
  tray.AddModule("FFSource", "source");
  tray.AddModule("Nothing", "nothing");
  tray.AddModule("FFCounter", "counter");
  
  tray.Execute(12);

  ENSURE_EQUAL(counts["counter"][I3Frame::Geometry], 3u);
  ENSURE_EQUAL(counts["counter"][I3Frame::Calibration], 3u);
  ENSURE_EQUAL(counts["counter"][I3Frame::DetectorStatus], 3u);
  ENSURE_EQUAL(counts["counter"][I3Frame::Physics], 3u);
}

TEST(forked)
{
  I3Tray tray;
  counts.clear();
  
  tray.AddModule("FFSource", "source");
  tray.AddModule("Nothing", "nothing");
  
  std::vector<std::string> params = 
    list_of("left")("middle")("right");
  tray.AddModule("Fork", "fork")
    ("Outboxes", params);

  tray.AddModule("FFCounter", "counterleft");
  tray.AddModule("FFCounter", "counterright");
  tray.AddModule("FFCounter", "countermiddle");
  
  
  

  tray.ConnectBoxes("source", "OutBox", "nothing");
  tray.ConnectBoxes("nothing", "OutBox", "fork");
  tray.ConnectBoxes("fork", "left", "counterleft");
  tray.ConnectBoxes("fork", "right", "counterright");
  tray.ConnectBoxes("fork", "middle", "countermiddle");

  tray.Execute(40);

  ENSURE_EQUAL(counts["counterleft"][I3Frame::Geometry], 10u);
  ENSURE_EQUAL(counts["counterleft"][I3Frame::Calibration], 10u);
  ENSURE_EQUAL(counts["counterleft"][I3Frame::DetectorStatus], 10u);
  ENSURE_EQUAL(counts["counterleft"][I3Frame::Physics], 10u);

  ENSURE_EQUAL(counts["counterright"][I3Frame::Geometry], 10u);
  ENSURE_EQUAL(counts["counterright"][I3Frame::Calibration], 10u);
  ENSURE_EQUAL(counts["counterright"][I3Frame::DetectorStatus], 10u);
  ENSURE_EQUAL(counts["counterright"][I3Frame::Physics], 10u);

  ENSURE_EQUAL(counts["countermiddle"][I3Frame::Geometry], 10u);
  ENSURE_EQUAL(counts["countermiddle"][I3Frame::Calibration], 10u);
  ENSURE_EQUAL(counts["countermiddle"][I3Frame::DetectorStatus], 10u);
  ENSURE_EQUAL(counts["countermiddle"][I3Frame::Physics], 10u);
}
