#!/usr/bin/env python
#
# 
#

from I3Tray import *
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# dump isn't connected to anything!
tray.AddModule("Dump","dump")

try:
    tray.Execute(5)
    sys.exit(1)
except:
    print "Good.  It threw."
    sys.exit(0) # indicate success.



