#!/usr/bin/env python3

from icecube.icetray import I3Tray


tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

tray.AddModule("AddNulls",
               where=["car", "sparetire", "ship", "bike", "bikehelmet"])

# verify they are there
tray.AddModule("FrameCheck",
               ensure_physics_has=["car", "sparetire", "ship", "bike", "bikehelmet"])


tray.AddModule("Keep",
               Keys=["car", "bike"],
               KeyStarts=["spare", "bi"])


tray.AddModule("FrameCheck",
               ensure_physics_has=["car", "bike", "sparetire", "bikehelmet"],
               ensure_physics_hasnt=["ship"])


tray.Execute(10)
