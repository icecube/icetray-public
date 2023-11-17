#!/usr/bin/env python3

from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import dataio 

tray = I3Tray()

# We'd better not *ever* come up with frame key names that contain
# backslashes, but I had to find out how many backslashes it takes in
# python get the C++ regex engine to match a literal backslash.  The
# answer is 3.  the C++ string has to be "\\", two backslashes, and
# python eats one of them.  
#
tray.AddModule("I3Reader","reader")(
    ("filename", "filtered.i3"),
    ("skipkeys", ["^[par]{3}.{5}\\?\\\\w$"])
    )

tray.AddModule("FrameCheck", "check1")(
    ("ensure_physics_has", ["DrivingTime", "I3EventHeader","InIceRawData", ]),
    ("ensure_physics_hasnt", ["IceTopRawData", "IceTopRecoHitSeries",
                              "InIceRecoHitSeries", "somewhere", r"particle?\w"])
    )

tray.AddModule("Dump", "dump")


tray.Execute()


