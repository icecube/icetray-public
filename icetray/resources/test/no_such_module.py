#!/usr/bin/env python

from I3Tray import *
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")
try:
    tray.AddModule("NoSuchModule")
except:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    # Failure: that should throw
    sys.exit(1)



