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
runfile = testdata + "/2006data/Run00089508.i3.gz"

tray = I3Tray()

tray.AddModule("I3Reader","i3reader")(
    ("Filename", runfile),
    ("SkipKeys",["I3PfFilterMask"])
    )

#
# Make sure that SkipKeys above worked
#
tray.AddModule("FrameCheck", "check",
               Ensure_Physics_Hasnt = ["I3PfFilterMask"])
#
# This is the very convenient "Dump" module which spits out the frames
# as they go by.  This is one of icecube's standard modules (in
# project icetray.  You get it for free, it's always available.)
#
tray.AddModule("Dump","dump")

#
# Here we specify how many frames to process, or we can omit the
# argument to Execute() and the the tray will run until a module tells
# it to stop (via RequestSuspension()).  We'll do a few frames so
# there's a chunk of data involved.
#
tray.Execute()

