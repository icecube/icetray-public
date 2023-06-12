#!/usr/bin/env python3

from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="one_event.i3")

tray.AddModule("Dump","dump")



tray.Execute()



