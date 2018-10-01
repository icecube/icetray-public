#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataclasses, dataio

from os.path import expandvars

import os
import sys

tray = I3Tray()

tray.AddModule("I3Reader","reader",
               Filename =  expandvars("pass1_default.i3.gz"))

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer",
               filename =  expandvars("pass1_best.i3.gz"),
               CompressionLevel =  9)



tray.Execute()

