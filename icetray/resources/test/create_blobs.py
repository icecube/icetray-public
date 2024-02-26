#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Run the CreateBlobs module and verify nothing horrible happens.
"""
from icecube.icetray import I3Tray

tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add("CreateBlobs")
tray.Execute(10)
