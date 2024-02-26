#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray
tray = I3Tray()

try:
    # generate empty frames
    tray.Add("BottomlessSource")
    tray.Add("AddNulls",
             where = ['foo'],
             If = ['this is', 'not a', 'function'])
    tray.Execute(10)

except Exception:
    print("ok, at that threw as expected.")
    sys.exit(0)
else:
    print("that should have thrown")
    sys.exit(1)




