#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
I3Tray should not call __bool__ on parameters where this can raise an exception.
"""

import sys
from icecube.icetray import I3Tray
from icecube import icetray

try:
    import numpy as np
except ImportError:
    sys.exit(0)


class ModdyMod(icetray.I3Module):
    def __init__(self, context):
        super().__init__(context)
        self.AddParameter("a", "a thing", None)

    def Configure(self):
        # we only care if GetParameter() fails
        self.GetParameter("a")

    def DAQ(self, frame):
        return


tray = I3Tray()

# bool(np.zeros(2)) is ambiguous and raises ValueError
tray.AddModule(ModdyMod, a=np.zeros(2))

tray.Execute(1)
