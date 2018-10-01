#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

#
# This sets up a bunch of paths of files and stuff.  Nice to have a
# real scripting language at one's disposal for this kind of thing.
#
testdata = expandvars("$I3_TESTDATA")
runfile = testdata + "/2007data/2007_I3Only_Run109732_Nch20.i3.gz"

tray = I3Tray()

tray.AddModule("I3Reader",
               Filename = runfile,
               SkipKeys = ["I3PfFilterMask","CalibratedFADC","CalibratedATWD"])

# This file is super old
tray.AddModule("QConverter")
tray.AddModule(lambda fr: False) # Drop all existing P-frames

#
# Make the Q frames into P
#
tray.AddModule("I3NullSplitter")

#
# And this is the magic writer.  We will make it work harder later.
#
tray.AddModule("I3Writer", filename =  "pass1.i3")

#
# Here we specify how many frames to process, or we can omit the
# argument to Execute() and the the tray will run until a module tells
# it to stop (via RequestSuspension()).  We'll do a few frames so
# there's a chunk of data involved.
#
tray.Execute(15)

