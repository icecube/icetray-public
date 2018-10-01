#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from I3Tray import I3Tray
from icecube import icetray
from icecube import dataclasses
from icecube import dataio
import os

class CountFrames(icetray.I3Module):
    """
    Module counts frames (by stream) of different kinds that pass by,
    reports if the observed counts do not match some supplied expected
    counts.  Used in testing the tray itself.
    
    *Parameters*:

    * **Counts** -  dictionary with key=Frame tag, value=count

    *Example*::

      tray.AddModule(icecube.util.modules.CountFrames, 'rrs',
                     Streams = {'P': 13, 'D' :1}
                     )

    This would check for 13 physics streams and 1 detectorstatus stream                     

    """
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("Counts",
                          "Counts we should look for",
                          {})

        self.AddOutBox("OutBox")

    def Configure(self):
        self.expected_counts = self.GetParameter("Counts")
        self.observed_counts = {}

    def Process(self):
        frame = self.PopFrame()
        tag = frame.Stop.id
        if tag not in self.observed_counts:
            self.observed_counts[tag] = 1
        else:
            self.observed_counts[tag] += 1

        self.PushFrame(frame)

    def Finish(self):
        print(self.observed_counts)
        print(self.expected_counts)
        assert(self.observed_counts == self.expected_counts, \
               "CountFrames error:\n  Expected:    %s\n  Observed:    %s" % (self.expected_counts, self.observed_counts))

tags = ['a', 'b', 'c', 'd', 'e', 'f', 'g']

index = 0;

fname = 'alpha.i3'

if os.path.exists(fname) and os.path.isfile(fname):
    os.unlink(fname)
    
i3f = dataio.I3File(fname, 'w')

for tag in tags:
    theframe = icetray.I3Frame(icetray.I3Frame.Stream(tag))
    theframe[tag] = icetray.I3Int(ord(tag))
    i3f.push(theframe)
    index += 1
i3f.close()

tray = I3Tray()

tray.AddModule("I3Reader", "reader",
               filename = fname)

tray.AddModule("Dump", "dump")

tray.AddModule(CountFrames, 'count',
               Counts = dict([(x, 1) for x in tags])
               )



tray.Execute()


