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
load("libmutineer")

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="pass1.i3")

tray.AddModule("MutineerModule", "mutineer")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer")(
    ("filename", "hasmutineer.i3.gz")
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
