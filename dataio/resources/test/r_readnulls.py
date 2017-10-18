#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=expandvars("withnulls.i3"))

tray.AddModule("Dump","dump")



tray.Execute()

