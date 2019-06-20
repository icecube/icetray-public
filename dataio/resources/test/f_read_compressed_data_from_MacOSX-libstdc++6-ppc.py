#!/usr/bin/env python

import os
import sys

from I3Tray import I3Tray
from os.path import expandvars

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()
tray.AddModule("I3Reader", Filename=expandvars("$I3_TESTDATA/dataio/olddata/string-21/MacOSX-libstdc++6-ppc.i3.gz"))
tray.Execute()

