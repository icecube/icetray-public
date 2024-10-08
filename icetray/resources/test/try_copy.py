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
tray.Add("BottomlessSource")

tray.Add("AddNulls", where = ["src1", "src2"])

# verify they are there
tray.Add("FrameCheck",
    ensure_physics_has = ["src1", "src2"],
    ensure_physics_hasnt = ["dst1", "dst2"])

tray.Add("Copy",
    Keys = ["src1", "dst1", "src2", "dst2"])

tray.Add("FrameCheck",
    ensure_physics_has = ["src1", "dst1", "src2", "dst2"])

# print em.   lots of nulls, dump shouldn't crash.
tray.Add("Dump","dump")

tray.Execute(15)

