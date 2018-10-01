#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray
from icecube import dataio
import os

tray = I3Tray()

tray.Add("BottomlessSource")

tray.Add("I3Writer", Filename = "foo.i3")               

tray.Execute(1)


for i in range(10):
    
    tray = I3Tray()

    tray.Add("I3Reader", Filename = "foo.i3")                   

    tray.Add("I3Writer", Filename = "foonew.i3")                   

    tray.Execute()
    os.unlink("foo.i3")
    os.rename("foonew.i3", "foo.i3")

f = dataio.I3File("foo.i3")
frame = f.pop_frame(icetray.I3Frame.TrayInfo)
print(frame)
for k in frame.keys():
    print("KEYS: %s" % k)

assert len(frame.keys()) == 11, "eleven keys expected in frame"

frame = f.pop_frame()

assert not frame, "only one frame expected in file"

fname = "foo.i3"
if os.path.exists(fname):
	os.unlink(fname)
