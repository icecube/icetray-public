/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/Utility.h>

TEST_GROUP(FrameFlow);

struct FFSource : public I3Module
{
  unsigned nextstop;

  FFSource(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");
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
  {
    AddOutBox("OutBox");
  }
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
  {
    AddOutBox("OutBox");
  }
  
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
  tray.AddModule("TrashCan", "trash");
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
  tray.AddModule("Fork", "fork")
    ("Outboxes", to_vector("left", "middle", "right"));

  tray.AddModule("FFCounter", "counterleft");
  tray.AddModule("FFCounter", "counterright");
  tray.AddModule("FFCounter", "countermiddle");
  tray.AddModule("TrashCan",  "trashleft");
  tray.AddModule("TrashCan",  "trashmiddle");
  tray.AddModule("TrashCan",  "trashright");

  tray.ConnectBoxes("source", "OutBox", "nothing");
  tray.ConnectBoxes("nothing", "OutBox", "fork");
  tray.ConnectBoxes("fork", "left", "counterleft");
  tray.ConnectBoxes("fork", "right", "counterright");
  tray.ConnectBoxes("fork", "middle", "countermiddle");
  tray.ConnectBoxes("counterleft", "OutBox", "trashleft");
  tray.ConnectBoxes("counterright", "OutBox", "trashright");
  tray.ConnectBoxes("countermiddle", "OutBox", "trashmiddle");

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
