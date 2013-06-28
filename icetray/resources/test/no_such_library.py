#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
tray = I3Tray()

try:
    from icecube import no_such_library 

    # generate empty frames
    tray.AddModule("BottomlessSource","bottomless")

    # print em
    tray.AddModule("NoSuchModule","dump")

    tray.Execute(5)
except:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    print("should have thrown")
    sys.exit(1)



