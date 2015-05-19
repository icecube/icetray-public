#!/usr/bin/env python

from I3Tray import *
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader","reader",
               Filename = "hasmutineer.i3.gz")

tray.AddModule("FrameCheck", "check1",
               Ensure_Physics_Has = ["mutineer"])

tray.AddModule("DeleteUnregistered", "du")

tray.AddModule("FrameCheck", "check2",
               Ensure_Physics_Hasnt = ["mutineer"])

tray.AddModule("Get", "getter")

tray.AddModule("Dump","dump")
tray.AddModule("CountFrames", "count",
               Physics = 10,
               Geometry = 1,
               Calibration = 1,
               DetectorStatus = 1)

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

fname = "hasmutineer.i3.gz"
if os.path.exists(fname):
    os.unlink(fname)

sys.exit(0)
