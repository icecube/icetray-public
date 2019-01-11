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
load("libphys-services")
load("libdataio")

tray = I3Tray()

tray.AddService("I3ReaderServiceFactory","gcdreader")(
    ("Filename", "split.gcd.i3"),
    ("OmitEvent",True)
    )

tray.AddService("I3EmptyStreamsFactory","empty_streams")(
    ("InstallGeometry",False),
    ("InstallCalibration",False),
    ("InstallStatus",False),
    ("EventTimeYear",2006),
    ("EventTimeNnanosec",209120770000000001)
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

# Clean up .i3 files rom earlier tests
file_list = glob("split.*.i3")

for f in file_list:
    os.unlink(f)
