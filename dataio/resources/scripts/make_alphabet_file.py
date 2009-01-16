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

i3f = dataio.I3File("alphabet.i3", dataio.I3File.Mode.Writing)

streams = [chr(x) for x in
           range(ord('a'), ord('z')+1)
           + range(ord('A'), ord('Z')+1) + range(ord('0'), ord('9')+1)]

for st in streams:
    print "=====", st, "====="
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(st))
    theframe[st] = icetray.I3Int(ord(st))

    i3f.push(theframe)

i3f.close()

for frame in dataio.I3File("alphabet.i3"):
    print frame
    assert frame.GetStop().id == streams[0]
    streams = streams[1:]
