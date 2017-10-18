#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

# We'd better not *ever* come up with frame key names that contain
# backslashes, but I had to find out how many backslashes it takes in
# python get the C++ regex engine to match a literal backslash.  The
# answer is 3.  the C++ string has to be "\\", two backslashes, and
# python eats one of them.  
#

tray.AddModule("I3Reader", "reader", Filename="filtered.i3",
    SkipKeys=["^[par]{3}.{5}\?\\\w$"])

tray.AddModule("FrameCheck", "check1")(
    ("ensure_physics_has", ["DrivingTime", "I3EventHeader","InIceRawData", ]),
    ("ensure_physics_hasnt", ["IceTopRawData", "IceTopRecoHitSeries",
                              "InIceRecoHitSeries", "somewhere", "particle?\w"])
    )



tray.Execute()


