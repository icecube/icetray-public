#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube import icetray, examples
from I3Tray import *

tray = I3Tray()

i3int = icetray.I3Int(777)

# generate empty frames
tray.AddModule("GetI3Int","giint",
               obj=i3int)

tray.Execute(1)

# see ya.
tray.Finish()
