#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from icecube import icetray, dataclasses, dataio
from I3Tray import I3Tray
import sys

#
# Generate .i3 file
#

i3f = dataio.I3File("tmp.i3", dataio.I3File.Mode.Writing)

streams = ['A', 'B', 'C', 'D', 'E', 'F']

for st in streams:
    print "=====", st, "====="
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(st))
    theframe[st] = icetray.I3Int(ord(st))

    i3f.push(theframe)

i3f.close()

#
#  Here's the tray
#
tray = I3Tray()

tray.AddModule('I3Reader', 'rd',
               Filename = 'tmp.i3')

def p(frame):
    print "FFFFFFF:", frame
    
tray.AddModule(p, 'dmp',
               Streams = [icetray.I3Frame.Stream(a) for a in streams])

tray.AddModule('I3Writer', 'wr',
               Filename = 'tmp2.i3')

tray.AddModule('TrashCan', 'tc')

tray.Execute()
tray.Finish()

#
#   Now read 'em back, verify that the frames are 'clean'
#
i3f = dataio.I3File("tmp2.i3", dataio.I3File.Mode.Reading)

f = i3f.pop_frame(icetray.I3Frame.Stream('I'))

streams = ['A', 'B', 'C', 'D', 'E', 'F']
for st in streams:
    theframe = i3f.pop_frame(icetray.I3Frame.Stream(st))
    print theframe
    assert len(theframe) == 1
    assert st in theframe
    i3i = theframe[st]
    assert i3i.value == ord(st)

assert not i3f.more() 
i3f.close()


