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
tray.AddModule("BottomlessSource")

tray.AddModule("AddNulls",
    where = ["here", "there", "everywhere", "tonsastuff"])

# verify they are there
tray.AddModule("FrameCheck",
    ensure_physics_has = ["here", "there", "everywhere", "tonsastuff"])


tray.AddModule("Delete",
    Keys = ["here", "there"])

tray.AddModule("FrameCheck",
    ensure_physics_has =  ["everywhere", "tonsastuff"],
    ensure_physics_hasnt =  ["here", "there"])

# print em.   lots of nulls, dump shouldn't crash.
tray.AddModule("Dump","dump")

tray.Execute(15)

