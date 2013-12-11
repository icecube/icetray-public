#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import I3Tray
from icecube import dataio
from icecube.examples.mutineer import MutineerModule

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="pass1.i3")

tray.AddModule(MutineerModule, "mutineer")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer")(
    ("filename", "hasmutineer.i3.gz")
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
