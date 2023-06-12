#!/usr/bin/env python3

import sys
from icecube.icetray import I3Tray
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")
try:
    tray.AddModule("NoSuchModule")
except Exception:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    # Failure: that should throw
    sys.exit(1)



