#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

class C:
    pass

try:
    tray.AddModule(C, "mod",
                   Startfrom = 0,
                   SumShouldBe = 6)
    print "That should have thrown"
    sys.exit(1)
except:
    print "good, it threw."
    sys.exit(0)


