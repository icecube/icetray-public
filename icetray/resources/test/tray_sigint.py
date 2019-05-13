#!/usr/bin/env python
#
#  Check that we raise an exception if we call SIGINT once
#
import sys
import os
import signal

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()


class mod(I3Module):
    def Process(self):
        os.kill(os.getpid(), signal.SIGINT)

tray.Add(mod)

try:
    tray.Execute(1)

    print("that should have thrown")
    sys.exit(1)

except KeyboardInterrupt as e:
    print("As expected, caught '%s'" % e)


