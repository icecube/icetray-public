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

i3f = dataio.I3File("test.i3", dataio.I3File.Mode.Writing)

frames = [('a', 0),
          ('b', 1),
          ('a', 2),
          ('b', 3),
          ('c', 4),
          ('c', 5),
          ('a', 6),
          ('c', 7)]

for pr in frames:
    frame = icetray.I3Frame(icetray.I3Frame.Stream(pr[0]))
    frame[pr[0]] = icetray.I3Int(pr[1])
    i3f.push(frame)

i3f.close()

browsable_file = dataio.I3BrowsableFile()
browsable_file.open_file('test.i3')

frame = browsable_file.get_frame(0)
assert frame['a'].value == 0
assert len(frame) == 1
assert frame.GetStop().id == 'a'

frame = browsable_file.get_frame(1)
assert frame['a'].value == 0
assert frame['b'].value == 1
assert len(frame) == 2
assert frame.GetStop().id == 'b'

frame = browsable_file.get_frame(2)
assert frame['a'].value == 2
assert frame['b'].value == 1
assert len(frame) == 2
assert frame.GetStop().id == 'a'

frame = browsable_file.get_frame(3)
assert frame['a'].value == 2
assert frame['b'].value == 3
assert len(frame) == 2
assert frame.GetStop().id == 'b'

frame = browsable_file.get_frame(4)
assert frame['a'].value == 2
assert frame['b'].value == 3
assert frame['c'].value == 4
assert len(frame) == 3
assert frame.GetStop().id == 'c'

frame = browsable_file.get_frame(5)
assert frame['a'].value == 2
assert frame['b'].value == 3
assert frame['c'].value == 5
assert len(frame) == 3
assert frame.GetStop().id == 'c'

frame = browsable_file.get_frame(6)
assert frame['a'].value == 6
assert frame['b'].value == 3
assert frame['c'].value == 5
assert len(frame) == 3
assert frame.GetStop().id == 'a'

frame = browsable_file.get_frame(7)
assert frame['a'].value == 6
assert frame['b'].value == 3
assert frame['c'].value == 7
assert len(frame) == 3
assert frame.GetStop().id == 'c'

