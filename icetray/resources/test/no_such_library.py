#!/usr/bin/env python

from I3Tray import *
tray = I3Tray()

try:
    from icecube import no_such_library 
    tray.AddModule("BottomlessSource")
    tray.AddModule("NoSuchModule")

    tray.Execute(5)
except:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    print("should have thrown")
    sys.exit(1)



