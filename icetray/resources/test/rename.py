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

tray.AddModule("Dump")

tray.AddModule("Rename",
    Keys = ["here", "here2", "there", "there2"])

# silently skip over these.
# can't rename what you can't find.
tray.AddModule("Rename", Keys = ["dne", "foo"])

tray.AddModule("FrameCheck",
    ensure_physics_has = ["here2", "there2", "everywhere", "tonsastuff"],
    ensure_physics_hasnt = ["here", "there"])

# print em.   lots of nulls, dump shouldn't crash.
tray.AddModule("Dump","dump")

tray.Execute(215)

print("gonna get usage")

u = tray.Usage()

print("Got Usage: %s" % u)

print("*****\n"*4)
for entry in u.items():
    print("%25s => %s" % entry)
print("*****\n"*4)

print("Total system time= %f s" % sum(entry.systime for entry in u.values()))
print("Total user time=   %f s" % sum(entry.usertime for entry in u.values()))
print("Max ncall=         %f s" % max(entry.ncall for entry in u.values()))

