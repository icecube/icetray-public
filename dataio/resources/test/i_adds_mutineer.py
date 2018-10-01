#!/usr/bin/env python
from I3Tray import I3Tray
from icecube import dataio
from icecube import dataclasses

from icecube.test_unregistered import UnregisteredTrack

tray = I3Tray()
tray.AddModule("I3Reader", Filename="pass1.i3")
tray.AddModule(lambda frame : \
               frame.Put("mutineer", UnregisteredTrack()))
tray.AddModule("Dump")
tray.AddModule("I3Writer", Filename = "hasmutineer.i3.gz")

tray.Execute()

