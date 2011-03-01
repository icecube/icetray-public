#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

load("libdataclasses")
load("libphys-services")
load("libdataio")

tray = I3Tray()

tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("Filename", "pass1.i3")
    )

tray.AddModule("I3Muxer","muxme")

tray.AddModule("Dump","dump")

tray.AddModule("I3MultiWriter","writer")(
    ("filename", "testm.%04u.i3.gz"),  
    ("sizelimit", 1) # this limit will do one-frame-per-file
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

filecount_is_correct = (len(glob("testm.????.i3.gz")) == 15)

if filecount_is_correct:
    sys.exit(0)
else:
    print "generated file count is incorrect"
    sys.exit(1)


