#!/usr/bin/env python
#
#  verify that the previous script was able to write a file containing
#  objects that it didn't have serialization code for
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
tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("Filename", "stillhasmutineer.i3.gz"),
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("FrameCheck", "checker")(
    ("Ensure_Physics_Has", ["mutineer"])
    )

tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

sys.exit(0)
