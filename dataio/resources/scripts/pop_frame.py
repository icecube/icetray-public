#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from icecube import icetray, dataclasses, dataio
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

i3f2 = dataio.I3File("tmp.i3")

frame = i3f2.pop_frame(icetray.I3Frame.None)

print frame
assert frame
stop =  frame.GetStop()
assert stop.id == 'A'

frame = i3f2.pop_frame()
assert frame
print frame
assert frame.GetStop().id == 'B'

frame = i3f2.pop_frame(icetray.I3Frame.Stream('C'))
assert frame
print frame
assert frame.GetStop().id == 'C'

frame = i3f2.pop_frame()
assert frame
print frame
assert frame.GetStop().id == 'D'






