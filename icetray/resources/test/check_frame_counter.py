#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Checking that the driving time exists
#
from icecube.icetray import I3Tray


tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

tray.AddModule("AddNulls", "stuff")(
    ("where", ["foo", "bar", "baz"])
    )

tray.AddModule("CountFrames", "count")(
    ("Physics", 11)
    )

# print em
tray.AddModule("Dump","dump")

# check whether frame element 'foo' exists (it shouldn't)
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_has", ["foo", "bar", "baz"]),
    )

tray.Execute(11)
