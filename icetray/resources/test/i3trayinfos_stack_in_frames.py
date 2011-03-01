#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, util
from I3Tray import *
import os

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")

tray.AddModule("I3Writer", "writer",
               Filename = "foo.i3")

tray.AddModule("TrashCan", "tc")

tray.Execute(1)
tray.Finish()


for i in range(10):
    
    tray = I3Tray()

    tray.AddModule("I3Reader", "read",
                   Filename = "foo.i3")

    tray.AddModule("I3Writer", "writer",
                   Filename = "foonew.i3")

    tray.AddModule("TrashCan", "tc")

    tray.Execute()
    tray.Finish()
    os.unlink("foo.i3")
    os.rename("foonew.i3", "foo.i3")

f = dataio.I3File("foo.i3")
frame = f.pop_frame(icetray.I3Frame.TrayInfo)
print frame
for k in frame.keys():
    print "KEYS:", k

assert len(frame.keys()) == 11, "eleven keys expected in frame"

frame = f.pop_frame()

assert not frame, "only one frame expected in file"

