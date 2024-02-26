#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray

tray = I3Tray()

class C:
    pass

try:
    tray.AddModule(C,
                   Startfrom = 0,
                   SumShouldBe = 6)
except Exception:
    print("good, it threw.")
    sys.exit(0)
else:
    print("That should have thrown")
    sys.exit(1)


