#!/usr/bin/env python

#
#
#   FIXME:  this verifies that if you cat together a bunch of .i3.gz files
#   and try to run on them the reader will only see the first one.
#   This is ticket #145
#
#

from I3Tray import *
from icecube import icetray, dataclasses, dataio
import os

#
# Generate .i3 file
#

tags = ['a', 'b', 'c', 'd', 'e']

index = 0;

for tag in tags:
    i3f = dataio.I3File("catme_%s.i3.gz" % tag, 'w')
    theframe = icetray.I3Frame(icetray.I3Frame.Physics)
    theframe[tag] = icetray.I3Int(ord(tag))
    theframe['index'] = icetray.I3Int(index)
    i3f.push(theframe)
    i3f.close()
    index += 1

if os.path.exists("catted.i3.gz") and os.path.isfile("catted.i3.gz"):
    os.unlink("catted.i3.gz")
    
cmd = "cat " + ' '.join(["catme_%s.i3.gz" % tag for tag in tags]) + " > catted.i3.gz"
print(cmd)
status = os.system(cmd)

for tag in tags:
    if os.path.exists("catme_%s.i3.gz" % tag):
        os.unlink("catme_%s.i3.gz" % tag)

tray = I3Tray()

tray.AddModule("I3Reader", "reader",
               filename = "catted.i3.gz")

tray.AddModule("Dump", "dump")

tray.Execute()


#for frame in dataio.I3File("alphabet.i3"):
#    print frame
#    assert frame.GetStop().id == streams[0]
#    streams = streams[1:]
