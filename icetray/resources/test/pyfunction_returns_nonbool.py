#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray

tray = I3Tray()


# can't convert that return value to bool or none
def f(frame):
    return { "three" : 3 }

try:
    tray.AddModule("BottomlessSource")
    tray.AddModule(f)
    tray.Execute()
except Exception as e:
    print("As expected, caught '%s'" % e)
else:
    print("that should have thrown")
    sys.exit(1)


