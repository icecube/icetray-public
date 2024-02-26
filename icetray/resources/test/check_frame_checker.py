#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray


tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

# print em
tray.AddModule("Dump")

# check whether frame element 'foo' exists (it shouldn't)
tray.AddModule("FrameCheck",
    ensure_physics_hasnt =["foo", "bar", "baz"],
    ensure_physics_has = ["this-will-cause-error"])

try:
    # This should fail
    tray.Execute(5)
except Exception:
    print("ok: failure as expected.")
    # ok it failed
    sys.exit(0)
else:
    sys.exit(1)

