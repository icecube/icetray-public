#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from I3Tray import I3Tray
from icecube import icetray
from icecube import dataio
import os

index = 0;

fname = 'ints.i3'

if os.path.exists(fname) and os.path.isfile(fname):
    os.unlink(fname)
    
i3f = dataio.I3File(fname, 'w')

for i in range(100):
    frame = icetray.I3Frame(icetray.I3Frame.Physics)
    frame['index'] = icetray.I3Int(i)
    i3f.push(frame)

i3f.close()

tray = I3Tray()

tray.AddModule("I3Reader", "reader",
               filename = fname)

tray.AddModule("I3Writer", "writer",
               Filename = "evens.i3",
               If = lambda f: f['index'].value % 2 == 0)

tray.AddModule("Dump", "dump",
               If = lambda f: f['index'].value % 2 == 0)



tray.Execute()



i = 0
for frame in dataio.I3File("evens.i3", 'r'):
    print(frame)
    if frame.Stop == icetray.I3Frame.Physics:
        assert frame['index'].value == i
        i += 2

fname = "evens.i3"
if os.path.exists(fname) and os.path.isfile(fname):
    os.unlink(fname)
