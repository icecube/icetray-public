#!/usr/bin/env python
#

import sys
from icecube import icetray, dataclasses, dataio
from I3Tray import *

tray = I3Tray()
tray.AddModule("I3Reader", "reader", Filename = "pass1.i3")
i = 0
j = 0
def drop_some_pframes(fr):
	global i, j
	i += 1
	if i == 1:
		return False
	if i > 5 and i < 8:
		return False
	if i > 10:
		return False
	j += 1
	return True
tray.AddModule(drop_some_pframes, "drop")
tray.AddModule("Dump","dump")
tray.AddModule("I3Writer","writer", Filename = "orphanarium.i3",
    DropOrphanStreams=[icetray.I3Frame.Calibration, icetray.I3Frame.DAQ])

tray.Execute()


tray = I3Tray()
tray.AddModule("I3Reader", "reader", Filename = "orphanarium.i3")
q = 0
p = 0
def count_q(fr):
	global q
	q += 1
def count_p(fr):
	global p
	p += 1
tray.AddModule(count_q, "count_q", Streams=[icetray.I3Frame.DAQ])
tray.AddModule(count_p, "count_p", Streams=[icetray.I3Frame.Physics])

tray.Execute()


if p != j:
	print("Number of P frames incorrect (%d != %d)" % (p, j))
	sys.exit(1)
if q != j:
	print("Number of Q frames incorrect (%d != %d)" % (q, j))
	sys.exit(1)
sys.exit(0)

