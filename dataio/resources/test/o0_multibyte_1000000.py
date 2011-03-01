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

tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("Filename", "pass1.i3")
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("Dump","dump")

tray.AddModule("I3MultiWriter","writer")(
    ("filename", "testmulti.%04u.i3"),
    ("sizelimit", 10**6) # 10^6 bytes
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()


