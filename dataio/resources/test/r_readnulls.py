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

tray.AddModule("I3Reader", "reader", Filename=expandvars("withnulls.i3"))

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
