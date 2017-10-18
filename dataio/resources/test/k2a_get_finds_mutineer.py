#!/usr/bin/env python

from I3Tray import *
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader","reader",
               Filename = "hasmutineer.i3.gz")

tray.AddModule("FrameCheck", "check1",
               Ensure_Physics_Has = ["mutineer"])

tray.AddModule("Get", "getter")

tray.AddModule("Dump","dump")



try:
    tray.Execute()
except:
    print("It threw as expected")
    sys.exit(0)
else:
    print("That should have thrown")
    sys.exit(1)



