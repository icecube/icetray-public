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

tray = I3Tray()

tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("Filename", "pass1.i3")
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("AddNulls", "nulls")(
    ("where", ["null1", "null2", "null3"])
    );

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer")(
    ("filename", "withnulls.i3")
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
