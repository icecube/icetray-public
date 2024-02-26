#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Simplest possible icetray script.
#

from icecube.icetray import I3Tray
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

# print em
tray.AddModule("Dump")

# do it 5 times.
tray.Execute(5)

tray.PrintUsage()
