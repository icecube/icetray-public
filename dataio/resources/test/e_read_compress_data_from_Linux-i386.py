#!/usr/bin/env python

from I3Tray import *
from os.path import expandvars

import os
import sys

from icecube import icetray, dataclasses, phys_services, dataio

tray = I3Tray()

tray.AddModule("I3Reader","reader", Filename = expandvars("$I3_TESTDATA/string-21/Linux-i386.i3.gz"))

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer")(
    ("filename", "pass1_fastest.i3.gz"),
    ("CompressionLevel", 1)
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
