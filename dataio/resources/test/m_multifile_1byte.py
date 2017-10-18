#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="pass1.i3")

tray.AddModule("Dump","dump")

tray.AddModule("I3MultiWriter","writer")(
    ("filename", "testm.%04u.i3.gz"),  
    ("sizelimit", 1), # this limit will do one-frame-per-file
    ("metadatastreams", []) # don't care about GCD per-file
    )



tray.Execute()


filecount_is_correct = (len(glob("testm.????.i3.gz")) == 15)

if filecount_is_correct:
    sys.exit(0)
else:
    print("generated file count is incorrect")
    sys.exit(1)


