#!/usr/bin/env python3

from icecube.icetray import I3Tray
from icecube import icetray, dataclasses, phys_services, dataio
from os.path import expandvars

import os
import sys

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename =  "pass1_best.i3.bz2")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer",
               filename =  "pass1_fastest.i3.bz2",
               CompressionLevel =  1)



tray.Execute()

