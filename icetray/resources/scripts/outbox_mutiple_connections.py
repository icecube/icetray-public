#!/usr/bin/env python
#
# 
#

from I3Tray import *
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# 2 dumps, both
tray.AddModule("Dump","dump1")
tray.AddModule("Dump","dump2")
tray.AddModule("Trash","trash")

try:
    tray.ConnectBoxes("bottomless","OutBox","dump1")
    tray.ConnectBoxes("dump1","OutBox","dump2")
    tray.ConnectBoxes("dump1","OutBox","trash")
except:
    print "OK: THROW EXPECTED"
    sys.exit(0)
else:
    print "ERROR: no throw as expected"
    sys.exit(1)



