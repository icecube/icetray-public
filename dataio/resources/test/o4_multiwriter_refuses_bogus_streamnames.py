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

tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["FIZZICKS"]),
    ("sizelimit", 10**5)
    )



try:
    tray.Execute()
    
except:
    sys.exit(0)
else:
    print("egh, module should have refused that parameter")
    sys.exit(1)



