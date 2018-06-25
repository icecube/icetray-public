#!/usr/bin/env python

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

try:
    tray.AddModule(3, 
                   Startfrom = 0,
                   SumShouldBe = 6)
except:
    print("good, it threw.")
    sys.exit(0)
else:
    print("That should have thrown")
    sys.exit(1)
