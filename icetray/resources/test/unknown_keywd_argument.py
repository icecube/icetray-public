#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray

tray = I3Tray()


tray.AddModule("BottomlessSource")


# can't convert that return value to bool or none
def f(frame, foo='foo', bar='bar', baz='baz'):
    print(foo, bar, baz)
    assert foo == 'foo', "foo didn't get set"
    assert bar == 'BARWASSET', "bar didn't get set"
    assert baz == 'baz', "baz didn't get set"
    return True


try:
    tray.AddModule(f, "keywordfn", bar='BARWASSET', argdoesntexist='oopsie')
    print("That should have thrown")
    sys.exit(1)

except RuntimeError as e:
    print(e)
    print("OK, threw as expected")
