#!/usr/bin/env python3

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#

import os
from tempfile import NamedTemporaryFile

from icecube.icetray import I3Tray
from icecube import dataio, icetray


class Gitter(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)

    def OnB(self, frame):
        print("On b, %s" % frame)
        assert frame.Stop == icetray.I3Frame.Stream('B')
        assert 'A' in frame
        assert 'B' in frame
        assert 'C' not in frame
        assert 'D' not in frame

    def OnF(self, frame):
        print("On f, %s" % frame)
        assert frame.Stop == icetray.I3Frame.Stream('F')
        assert 'A' in frame
        assert 'B' in frame
        assert 'C' in frame
        assert 'D' in frame
        assert 'E' in frame
        assert 'F' in frame

    def Configure(self):
        self.Register(icetray.I3Frame.Stream('B'), self.OnB)
        self.Register(icetray.I3Frame.Stream('F'), self.OnF)


with NamedTemporaryFile(delete=False) as f:
    i3f = dataio.I3File(f.name, 'w')

    streams = ['A', 'B', 'C', 'D', 'E', 'F']

    for st in streams:
        print("=====%s=====" % st)
        theframe = icetray.I3Frame(icetray.I3Frame.Stream(st))
        theframe[st] = icetray.I3Int(ord(st))

        i3f.push(theframe)

    i3f.close()

    #
    #  Here's the tray
    #
    tray = I3Tray()
    tray.AddModule('I3Reader', Filename=f.name)

    def p(frame):
        print("FFFFFFF: %s" % frame)

    tray.AddModule(p, Streams=[icetray.I3Frame.Stream(a) for a in streams])
    tray.AddModule(Gitter)
    tray.Execute()

    if os.path.exists(f.name):
        os.unlink(f.name)
