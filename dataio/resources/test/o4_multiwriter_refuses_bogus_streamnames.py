#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["FIZZICKS"]),
    ("sizelimit", 10**5)
    )

tray.AddModule("TrashCan", "the can");

try:
    tray.Execute()
    tray.Finish()
except:
    sys.exit(0)
else:
    print("egh, module should have refused that parameter")
    sys.exit(1)



