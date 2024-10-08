#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
import sys

try:
    import numpy
except ImportError:
    print('Numpy not found -- skipping test')
    sys.exit(0)

t = I3Tray()
t.Add("BottomlessSource")
t.Add(lambda x: numpy.True_)
t.Add("CountFrames", Physics = 15)
t.Add(lambda x: numpy.False_,)
t.Add("CountFrames", Physics = 0)
t.Execute(15)
t.Finish()
