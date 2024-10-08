#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray
tray = I3Tray()

try:
    from icecube import no_such_library  # noqa
    tray.AddModule("BottomlessSource")
    tray.AddModule("NoSuchModule")

    tray.Execute(5)
except Exception:
    print("Good.  It threw.")
    sys.exit(0) # indicate success.
else:
    print("should have thrown")
    sys.exit(1)



