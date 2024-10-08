#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
from icecube import icetray
from icecube.icetray.round_robin_streams import RoundRobinStreams

tray = I3Tray()

#
#  Will issue frames on the 4 rotating streams
#
streams = [icetray.I3Frame.Stream('X'),
           icetray.I3Frame.Stream('Y'),
           icetray.I3Frame.Stream('Z')]

tray.AddModule(RoundRobinStreams, "bs",
               Streams = streams
               )

#
#   Filter out frames the putint didn't see
#
seen = dict(X=0, Y=0, Z=0)

def fn(frame):
    print("running on frame %s" % frame)
    seen[frame.Stop.id] += 1

tray.AddModule(fn, "fn",
               Streams = streams)

tray.AddModule("Dump", "dump")

tray.Execute(9)

print(seen)
assert seen['X'] == 3
assert seen['Y'] == 3
assert seen['Z'] == 3


