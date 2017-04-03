#!/usr/bin/env python

from I3Tray import *
from icecube import dataio
from os.path import expandvars
import os

infile = expandvars("$I3_TESTDATA")+"/2007data/2007_I3Only_Run109732_Nch20.i3.gz"
outfile = "async.i3"

count1=0
count2=0

all_frames=[
#	icetray.I3Frame.TrayInfo,
	icetray.I3Frame.Geometry,
	icetray.I3Frame.Calibration,
	icetray.I3Frame.DetectorStatus,
	icetray.I3Frame.DAQ,
	icetray.I3Frame.Physics,
]

tray1 = I3Tray()
tray1.AddModule("I3AsyncReader", filename=infile)
def mcount1(frame):
	global count1
	count1+=1
tray1.AddModule(mcount1, Streams=all_frames)
tray1.AddModule("I3AsyncWriter", filename=outfile, Streams=all_frames)
tray1.Execute()
tray1.Finish()
#del tray1 # make sure tray1 is really _really_ done

# verify that the resulting file can be read
tray2 = I3Tray()
tray2.AddModule("I3Reader", filename=outfile)
def mcount2(frame):
	global count2
	count2+=1
tray2.AddModule(mcount2, Streams=all_frames)
tray2.Execute()
tray2.Finish()

assert(count1==count2)
os.unlink(outfile)
