#!/usr/bin/env python3

from icecube.icetray import I3Tray

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
    Sizelimit = 3*10**5, # 300 KB
    MetadataStreams=[icetray.I3Frame.DetectorStatus]
    )



tray.Execute()



