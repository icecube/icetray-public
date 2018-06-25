#!/usr/bin/env python
#
#  Pass nonsensical python object to AddModule, expect log_fatal
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

class Notamodule:
    pass

try:
    tray.AddModule(Notamodule)
    print("that should have thrown")
    sys.exit(1)

except RuntimeError as e:
    print("As expected, caught '%s'" % e)

                   

