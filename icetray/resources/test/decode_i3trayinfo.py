#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray
from icecube import dataio
import os

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")

tray.AddModule("Dump", "d")

tray.AddModule("I3Writer", "writer",
               Filename = "foo.i3",
               If = lambda f: True
               )

tray.AddModule("TrashCan", "tc")

tray.Execute(1)
tray.Finish()

f = dataio.I3File("foo.i3")

frame = f.pop_frame(icetray.I3Frame.TrayInfo)
print(frame)
for k in frame.keys():
    print("Getting %s" % k)
    print(frame[k])

