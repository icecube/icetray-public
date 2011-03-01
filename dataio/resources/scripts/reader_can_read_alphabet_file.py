#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from I3Tray import *
from icecube import icetray, dataclasses, dataio, util
import os

#
# Generate .i3 file
#

tags = ['a', 'b', 'c', 'd', 'e', 'f', 'g']

index = 0;

fname = 'alpha.i3'

if os.path.exists(fname) and os.path.isfile(fname):
    os.unlink(fname)
    
i3f = dataio.I3File(fname, dataio.I3File.Mode.Writing)

for tag in tags:
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(tag))
    theframe[tag] = icetray.I3Int(ord(tag))
    i3f.push(theframe)
    index += 1
i3f.close()

tray = I3Tray()

tray.AddModule("I3Reader", "reader",
               filename = fname)

tray.AddModule("Dump", "dump")

tray.AddModule(util.modules.CountFrames, 'count',
               Counts = dict([(x, 1) for x in tags])
               )

tray.AddModule("TrashCan", "trashcan")

tray.Execute()
tray.Finish()

#for frame in dataio.I3File("alphabet.i3"):
#    print frame
#    assert frame.GetStop().id == streams[0]
#    streams = streams[1:]
