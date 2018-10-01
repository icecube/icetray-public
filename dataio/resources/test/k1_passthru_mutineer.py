#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
#  no longer!  Lazy frame to the rescue.  Unkown class should just flow thru

tray.AddModule("I3Reader", "reader", Filename="hasmutineer.i3.gz")

tray.AddModule("FrameCheck", "checker")(
    ("Ensure_Physics_Has", ["mutineer"])
    )

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer", "writer")(
    ("Filename", "stillhasmutineer.i3.gz")
    )


tray.Execute()

