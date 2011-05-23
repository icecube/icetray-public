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
load("libDOMcalibrator")
load("libdataio")
load("libexamples")

#
# This sets up a bunch of paths of files and stuff.  Nice to have a
# real scripting language at one's disposal for this kind of thing.
#
tools = expandvars("$I3_PORTS")
runfile = tools + "/test-data/2006data/Run00089508.i3.gz"

tray = I3Tray()

tray.AddModule("I3Reader","reader", Filename = runfile, SkipKeys = ["I3PfFilterMask"])

# This file is super old
tray.AddModule("QConverter", "qify")
tray.AddModule(lambda fr: False, "dropps") # Drop all existing P-frames

#
# A DOMCalibrator.  Obviously.
#
tray.AddModule("I3DOMcalibrator","merge")

#
# And an appropriately named but nonetheless cute feature
# extractor.
#
tray.AddModule("DumbFeatureExtractor","dumbfe")

#
# Make the Q frames into P
#
tray.AddModule("I3NullSplitter", "donothing")
 
#
# This would probably be better named I3ConstantSeed.  You can imagine
# what it does.
#
tray.AddModule("PutParticle","put",
               Zenith = 1.28,
               Azimuth = 3.14,
               Where =  "somewhere")

#
# This is the very convenient "Dump" module which spits out the frames
# as they go by.  This is one of icecube's standard modules (in
# project icetray.  You get it for free, it's always available.)
#
tray.AddModule("Dump","dump")

#
# And this is the magic writer.  We will make it work harder later.
#
tray.AddModule("I3Writer","writer",
               filename =  "pass1.i3"
               )

#
# The TrashCan is another standard module.  Every module's outboxes
# must be connected to something.  The I3Writer, above, sends things
# downstream after it has written them because it doesn't know if it
# really is going to be the last module in the chain.  This moduleb
# catches whatever comes through and just discards it.
#
tray.AddModule("TrashCan", "the can");

#
# Here we specify how many frames to process, or we can omit the
# argument to Execute() and the the tray will run until a module tells
# it to stop (via RequestSuspension()).  We'll do a few frames so
# there's a chunk of data involved.
#
tray.Execute()
tray.Finish()
