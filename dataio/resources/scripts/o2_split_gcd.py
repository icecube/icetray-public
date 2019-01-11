#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

load("libdataclasses")
load("libdataio")

tray = I3Tray()

tray.AddModule("I3Reader","reader")(
    ("Filename", "pass1.i3")
    )

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","gcdwriter")(
    ("filename", "split.gcd.i3"),
    ("streams", ["Geometry", "Calibration", "DetectorStatus"])
    )

tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["Physics"]),
    ("sizelimit", 10**5)
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

    


