#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *
from icecube import icetray, dataclasses, phys_services, dataio
from os.path import expandvars

import os
import sys

tray = I3Tray()

tray.AddService("I3ReaderServiceFactory","readerfactory",
                Filename =  "pass1_best.i3.gz")

tray.AddModule("I3Muxer","muxme")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer",
               filename =  "pass1_fastest.i3.gz",
               CompressionLevel =  1)

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
