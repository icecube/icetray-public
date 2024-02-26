#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray


tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

tray.AddModule("Rename", Keys = ["here"])

try:
    tray.Execute(1)
except RuntimeError:
    pass
else:
    raise Exception("This should have failed")

