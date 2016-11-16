#!/usr/bin/env python

"""
Run the CreateBlobs module and verify nothing horrible happens.
"""
from I3Tray import I3Tray
from icecube import icetray

tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add("CreateBlobs")
tray.Execute(10)
