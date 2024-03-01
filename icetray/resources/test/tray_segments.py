#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Check that tray segments work
#

from icecube.icetray import I3Tray

from icecube import icetray

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

addnullsaltconfig = icetray.module_altconfig("AddNulls", Where=["somestuff"])

@icetray.traysegment
def checkanddump(tray, name, checkfor=None):
    if checkfor is None:
        checkfor=["thisdoesntexist"]
    # verify they are there
    tray.AddModule("FrameCheck", name + "_checker",
                   ensure_physics_has=checkfor)
    tray.AddModule("Dump", name + "_dump")

@icetray.traysegment
def checkif(tray, name, If='blah'):
    if If == 'blah':
        raise Exception('If not passed through')

@icetray.timedtraysegment
def timethedump(tray, name):
    tray.AddModule("Dump", name+"_timeddump")
    
# Add the default alt-default config
tray.AddSegment(addnullsaltconfig)
# Add the alt-default config, but with one overridden
# Note: capitalization is deliberately wrong to test deduplication!
tray.AddSegment(addnullsaltconfig,
                where=["here", "there", "everywhere", "tonsastuff"])
tray.AddSegment(checkanddump, # Deliberate miscapitalization below
                CheckFor=["somestuff", "here", "there", "everywhere", "tonsastuff"])
tray.AddSegment(checkif, If=lambda fr: True)

# Add a timed dump
tray.AddSegment(timethedump, "timethedump")

def check_timer(frame, segment_name):
    if segment_name+"_SegmentTimer_seconds" not in frame:
        raise Exception("icetray.timedtraysegment didn't produce an output timer.")
    processing_time = frame[segment_name+"_SegmentTimer_seconds"].value
    if processing_time < 0:
        raise Exception("icetray.timedtraysegment returned a negative time.")
    return

tray.Add(check_timer, segment_name="timethedump",
         Streams = [icetray.I3Frame.DAQ, icetray.I3Frame.Physics])

tray.Execute(10)
