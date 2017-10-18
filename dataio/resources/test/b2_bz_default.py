#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3Reader","reader",
               Filename =  "pass1.i3")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer",
               filename =  expandvars("pass1_default.i3.bz2"))



tray.Execute()

