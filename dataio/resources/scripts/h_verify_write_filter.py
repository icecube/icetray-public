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
load("libexamples")

tray = I3Tray()

# We'd better not *ever* come up with frame key names that contain
# backslashes, but I had to find out how many backslashes it takes in
# python get the C++ regex engine to match a literal backslash.  The
# answer is 3.  the C++ string has to be "\\", two backslashes, and
# python eats one of them.  
#
tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("filename", "filtered.i3"),
    ("skipkeys", ["^[par]{3}.{5}\?\\\w$"]) 
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("FrameCheck", "check1")(
    ("ensure_physics_has", ["CalibratedATWD","CalibratedFADC", "DrivingTime", 
                            "I3EventHeader","InIceRawData", ]),
    ("ensure_physics_hasnt", ["IceTopRawData", "IceTopRecoHitSeries",
                              "InIceRecoHitSeries", "somewhere", "particle?\w"])
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

