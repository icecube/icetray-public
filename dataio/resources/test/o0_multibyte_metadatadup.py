#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import icetray, dataclasses, dataio

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="pass1.i3")
tray.AddModule("Dump","dump")
tray.AddModule("I3MultiWriter","writer",
    Filename = "testmultiD.%04u.i3",
    Sizelimit = 10**6, # 10^6 bytes
    MetadataStreams=[icetray.I3Frame.DetectorStatus]
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()


