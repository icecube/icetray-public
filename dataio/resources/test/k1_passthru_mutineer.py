#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys

load("libdataclasses")
load("libphys-services")
load("libdataio")

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
tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

sys.exit(0)
