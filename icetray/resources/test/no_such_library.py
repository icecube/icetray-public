#!/usr/bin/env python3

import sys
from I3Tray import I3Tray
tray = I3Tray()

try:
    from icecube import no_such_library  # noqa
    tray.AddModule("BottomlessSource")
    tray.AddModule("NoSuchModule")

    tray.Execute(5)
except Exception:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    print("should have thrown")
    sys.exit(1)



