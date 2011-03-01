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

tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("filename", "pass1.i3"),
    ("skipkeys", ["IceTop.*"])
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("FrameCheck", "check1")(
    ("ensure_physics_has", ["CalibratedATWD","CalibratedFADC","DrivingTime", 
                            "I3EventHeader",
                            "InIceRawData", "InIceRecoHitSeries", "somewhere"]),
    ("ensure_physics_hasnt", ["IceTopRawData", "IceTopRecoHitSeries"])
    )

#
# To verify filter-out of special chars
#
tray.AddModule("PutParticle", "put")(
    ("where","particle?\w")
    )

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer", "writer")(
    ("filename", "filtered.i3"),
    ("skipkeys", [".*Reco.*", "\w{4}where"])
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

