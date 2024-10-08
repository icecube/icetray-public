#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray
from icecube.icetray import I3Tray

tray = I3Tray()


tray.AddModule("BottomlessSource","bs")

def putints(frame, where, value):
    frame[where] = icetray.I3Int(value)
    print('put value %s at %s' % (value, where))

tray.AddModule(putints, 'putter',
               where = 'here',
               value = 777)

def checkints(frame, where, value):
    i3i = frame[where]
    assert i3i.value == value
    print('value %s ok' % value)

tray.AddModule(checkints, 'ci',
               where = 'here',
               value = 777)

tray.AddModule('Dump', 'dump')


tray.Execute(10)





