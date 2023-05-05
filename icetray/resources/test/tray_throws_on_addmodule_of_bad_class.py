#!/usr/bin/env python3
#
#  Pass nonsensical python object to AddModule, expect log_fatal
#

import sys
from I3Tray import I3Tray

tray = I3Tray()

class Notamodule:
    pass

try:
    tray.AddModule(Notamodule)
    print("that should have thrown")
    sys.exit(1)

except RuntimeError as e:
    print("As expected, caught '%s'" % e)

                   

