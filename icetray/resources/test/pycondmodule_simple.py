#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Sample i3module in python
#

from icecube.icetray import I3Tray
from icecube.icetray import I3ConditionalModule


class Mod(I3ConditionalModule):
    def __init__(self, context):
        I3ConditionalModule.__init__(self, context)

    def Configure(self):
        pass

    def Process(self):
        raise AssertionError()


tray = I3Tray()
# generate empty frames
tray.AddModule("BottomlessSource", "bottomless")
tray.AddModule(Mod, "PythonConditionalModule", If=lambda f: False)
tray.Execute(5)

print("Okay. Exiting.")
