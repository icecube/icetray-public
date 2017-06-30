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

i3f = dataio.I3File("alphabet.i3", 'w')

streams = [chr(x) for x in
           list(range(ord('a'), ord('z')+1))
           + list(range(ord('A'), ord('Z')+1)) + list(range(ord('0'), ord('9')+1))]

index = 0;

for st in streams:
    print("=====", st, "=====")
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(st))
    theframe[st] = icetray.I3Int(ord(st))
    theframe['index'] = icetray.I3Int(index)
    i3f.push(theframe)
    index += 1

i3f.close()

for frame in dataio.I3File("alphabet.i3"):
    print(frame)
    assert frame.Stop.id == streams[0]
    streams = streams[1:]
