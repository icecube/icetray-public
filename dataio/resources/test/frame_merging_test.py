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

i3f = dataio.I3File("tmp.i3", 'w')

streams = ['A', 'B', 'C', 'D', 'E', 'F'] * 10
counter = 1

for st in streams:
    print("=====%s=====" % st)
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(st))
    theframe[st] = icetray.I3Int(ord(st) * counter)
    counter += 1
    i3f.push(theframe)

i3f.close()

#
#  Here's the tray
#
tray = I3Tray()

tray.AddModule('I3Reader', 'rd',
               Filename = 'tmp.i3')

tray.AddModule("Dump", "dump")

def p(frame):
    print("FFFFFFF:%s" % frame)
    
tray.AddModule(p, 'dmp',
               Streams = [icetray.I3Frame.Stream(a) for a in streams])

tray.AddModule('I3Writer', 'wr',
               Filename = 'tmp2.i3')



tray.Execute()


#
#   Now read 'em back, verify that the frames are 'clean' and have correct values
#
i3f = dataio.I3File("tmp2.i3", 'r')

f = i3f.pop_frame(icetray.I3Frame.Stream('I'))

counter = 1
for st in streams:
    theframe = i3f.pop_frame(icetray.I3Frame.Stream(st))
    print(theframe)
    theframe.purge()
    print(theframe)
    assert len(theframe) == 1
    assert st in theframe
    i3i = theframe[st]
    print("%c ==>   %u =? %u" % (st, ord(st) * counter, i3i.value))
    assert i3i.value == ord(st) * counter
    print([thing.value for thing in list(theframe.values())])
    counter += 1
    

assert not i3f.more() 
i3f.close()


