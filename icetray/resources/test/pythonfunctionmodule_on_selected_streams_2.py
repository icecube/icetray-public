#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, util
from I3Tray import *
import os

tray = I3Tray()

#
#  Will issue frames on the 4 rotating streams
#
streams = [icetray.I3Frame.Stream('X'),
           icetray.I3Frame.Stream('Y'),
           icetray.I3Frame.Stream('Z')]

tray.AddModule(util.RoundRobinStreams, "bs",
               Streams = streams
               )

#
#   Filter out frames the putint didn't see
#
seen = dict(X=0, Y=0, Z=0)

def fn(frame):
    global seen
    print("running on frame %s" % frame)
    seen[frame.Stop.id] += 1
    
tray.AddModule(fn, "fn",
               Streams = streams)

tray.AddModule("Dump", "dump")

tray.AddModule("TrashCan", "tc")

tray.Execute(9)
tray.Finish()

print(seen)
assert seen['X'] == 3
assert seen['Y'] == 3
assert seen['Z'] == 3
    

