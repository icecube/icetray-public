#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray

tray = I3Tray()

tray.AddModule("BottomlessSource")

def f1(frame, x='x', y='y'):
    assert x == 'x'
    assert y == 'y'

tray.AddModule(f1, 'f1')

def f2(frame, x='x', y='y'):
    print("f2: %s %s" % (x, y))
    assert x == 'configured'
    assert y == 'y'

tray.AddModule(f2, 'f2',
               x = 'configured')

def f3(frame, x='x', y='y'):
    assert x == 'configured'
    assert y == 'configured'

tray.AddModule(f3, 'f3',
               x = 'configured',
               y = 'configured')

tray.Execute(1)
