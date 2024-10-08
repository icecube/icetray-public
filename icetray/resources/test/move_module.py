#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Test reordering of modules within an I3Tray.
"""

from icecube import icetray, dataio  # noqa: F401 # needed to add I3InfiniteSource
from icecube.icetray import I3Tray

def thing1(frame):
    frame['foo'] = icetray.I3Int(42)

def thing2(frame):
    if 'foo' not in frame:
        raise ValueError("Modules didn't run in the right order!")

def thing3(frame):
    frame['bar'] = icetray.I3Bool()

def run(hook=lambda tray: None):
    tray = I3Tray()
    tray.Add("I3InfiniteSource", Stream=icetray.I3Frame.Physics)
    tray.Add(thing3, "thing3")
    tray.Add(thing2, "thing2")
    tray.Add(thing1, "thing1")
    hook(tray)
    tray.Execute(1)

# Without reordering, thing2 should fail
try:
    run()
except Exception:
    pass
else:
        raise Exception("Module should have raised an error when run out-of-order")

# With reordering, everything is hunky-dory
run(lambda tray: tray.MoveModule('thing1', 'thing2'))

# Moving a nonexistent module fails
try:
    run(lambda tray: tray.MoveModule('no_name', 'thing2'))
except Exception:
    pass
else:
        raise Exception("Reordering should fail with nonexistent target module")

# Moving a module to before a nonexistent module also fails
try:
    run(lambda tray: tray.MoveModule('thing1', 'no_anchor'))
except Exception:
    pass
else:
        raise Exception("Reordering should fail with nonexistent anchor module")

