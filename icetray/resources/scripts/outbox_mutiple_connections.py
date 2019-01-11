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

tray.ConnectBoxes("bottomless","OutBox","dump1")
tray.ConnectBoxes("dump1","OutBox","dump2")
tray.ConnectBoxes("dump1","OutBox","trash")


try:
    tray.Execute(5)
    sys.exit(1)
except:
    print "Good.  It threw."
    sys.exit(0) # indicate success.



