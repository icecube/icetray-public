#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")


try:
    # print em
    tray.AddModule("NoSuchModule","dump")
except:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    # Failure: that should throw
    sys.exit(1)



