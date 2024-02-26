#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray

tray = I3Tray()

tray.AddModule("BottomlessSource")

# no If
def f1(frame):
    pass

tray.AddModule(f1)

# a basic If
def f2(frame):
    pass

tray.AddModule(f2,
               If = lambda fr:True)

# some other args and an If
def f3(frame, x='x', y='y'):
    assert x == 'configured'
    assert y == 'configured'

tray.AddModule(f3,
               x = 'configured',
               y = 'configured',
               If = lambda fr:True)

tray.Execute(1)
