#!/usr/bin/env python
#
# Big script with lots of branching and buffering
#

from I3Tray import I3Tray
tray = I3Tray()

nframes = 10000

# generate empty frames
tray.AddModule("IntGenerator","ig")
tray.AddModule("PhysicsBuffer", "b1",
    BufferSize =  6)
tray.ConnectBoxes("ig", "OutBox", "b1")

tray.AddModule("FrameCheck", "c1",
    ensure_physics_has =  ["myint"])
tray.ConnectBoxes("b1", "OutBox", "c1")

tray.AddModule("Fork", "f1",
    outboxes = ["one", "two", "three"])
tray.ConnectBoxes("c1", "OutBox", "f1")



tray.AddModule("AddNulls", "an1",
    where =  ["an1"])
tray.ConnectBoxes("f1", "one",    "an1")

tray.AddModule("PhysicsBuffer", "b2",
    BufferSize =  7)
tray.ConnectBoxes("an1", "OutBox", "b2")

tray.AddModule("FrameCheck", "c2",
    ensure_physics_has =  ["myint", "an1"])
tray.ConnectBoxes("b2", "OutBox", "c2")


tray.AddModule("AddNulls", "an2",
    where =  ["an2"])
tray.ConnectBoxes("f1", "two",    "an2")

tray.AddModule("PhysicsBuffer", "b3",
    BufferSize =  0)
tray.ConnectBoxes("an2", "OutBox", "b3")

tray.AddModule("Fork", "f2",
    Outboxes =  ["one", "two"])
tray.ConnectBoxes("b3", "OutBox", "f2")

tray.AddModule("AddNulls", "an5",
    where =  ["an5"])
tray.ConnectBoxes("f2", "one", "an5")

tray.AddModule("PhysicsBuffer", "a-b4",
    BufferSize =  1101)
tray.ConnectBoxes("an5", "OutBox", "a-b4")

tray.AddModule("FrameCheck", "a-c4",
    ensure_physics_has =  ["myint", "an5", "an2"],
    ensure_physics_hasnt =  ["an1", "an3", "an4", "an6"])
tray.ConnectBoxes("a-b4", "OutBox", "a-c4")

tray.AddModule("CountFrames", "a-cnt1", 
   physics =  nframes)
tray.ConnectBoxes("a-c4", "OutBox", "a-cnt1")


tray.AddModule("AddNulls", "an3",
    where =  ["an3"])
tray.ConnectBoxes("f1", "three",  "an3")

tray.AddModule("PhysicsBuffer", "b4",
    BufferSize =  211)
tray.ConnectBoxes("an3", "OutBox", "b4")

tray.AddModule("FrameCheck", "c4",
    ensure_physics_has =  ["myint", "an3"],
    ensure_physics_hasnt =  ["an5", "an1"])
tray.ConnectBoxes("b4", "OutBox", "c4")

tray.AddModule("CountFrames", "cnt1",
    physics =  nframes)
tray.ConnectBoxes("c4", "OutBox", "cnt1")

# do it nframes times.
tray.Execute(nframes)
