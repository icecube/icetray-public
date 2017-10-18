#!/usr/bin/env python

from icecube import icetray
from I3Tray import *
import sys

try:
	import numpy
except ImportError:
	print('Numpy not found -- skipping test')
	sys.exit(0)


# some parameter exercise

t = I3Tray()

t.AddModule("BottomlessSource", "src")

t.AddModule(lambda x: numpy.True_,"truemodule")
t.AddModule("CountFrames", "count1",
            Physics = 15)

t.AddModule(lambda x: numpy.False_, "falsemodule")
t.AddModule("CountFrames", "count2",
            Physics = 0)


t.Execute(15)
t.Finish()
