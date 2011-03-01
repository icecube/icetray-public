#!/usr/bin/env python

from icecube import icetray
from I3Tray import *
import numpy


# some parameter exercise

t = I3Tray()

t.AddModule("BottomlessSource", "src")

t.AddModule(lambda x: numpy.True_,"truemodule")
t.AddModule("CountFrames", "count1",
            Physics = 15)

t.AddModule(lambda x: numpy.False_, "falsemodule")
t.AddModule("CountFrames", "count2",
            Physics = 0)

t.AddModule("TrashCan", "tc")
t.Execute(15)
t.Finish()
