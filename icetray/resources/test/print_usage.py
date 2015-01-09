#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from math import pi
from math import cos
import random

from I3Tray import I3Tray
from icecube import icetray

from icecube.icetray.I3Test import ENSURE

tray = I3Tray()

class ComputeSomething(icetray.I3Module) :
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("N", "Number of times to compute something.", 0)
        self.AddOutBox("OutBox") # let's make the default "OutBox"
    def Configure(self):
        self.n = self.GetParameter("N")
    def Physics(self, frame):
        for i in range(self.n) :
            x = cos(random.uniform(0,2 * pi))
        self.PushFrame(frame)

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

tray.AddModule(ComputeSomething, n = 100000)
tray.AddModule(ComputeSomething, n = 10000)
tray.AddModule(ComputeSomething, n = 1000)
tray.AddModule(ComputeSomething, n = 100)
tray.AddModule(ComputeSomething, n = 10)
tray.AddModule(ComputeSomething, n = 1)

# do it 5 times.
tray.Execute(5)

# see ya.
tray.Finish()

printed_keys = tray.PrintUsage(fraction = 0.99999999)
ENSURE(len(printed_keys) > 2, "Not enough modules printed.")

printed_keys = tray.PrintUsage(fraction = 0.98)
ENSURE(len(printed_keys) == 2, "There should be only two modules printed. Got %d"\
       % len(printed_keys))

printed_keys = tray.PrintUsage(fraction = 0.8)
ENSURE(len(printed_keys) == 1, "There should be only one module printed. Got %d"\
       % len(printed_keys))

