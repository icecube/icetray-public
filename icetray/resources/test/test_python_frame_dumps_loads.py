#!/usr/bin/env python

#
#  if we create a frame, dump it to a
#  string in python and load it back into
#  a new frame, will we get the original back?
#
from icecube import icetray

frame = icetray.I3Frame(icetray.I3Frame.Physics)
frame["test_01"] = icetray.I3Int(42)

dump_str = frame.dumps()

del frame

frame = icetray.I3Frame()
frame.loads(dump_str)

if frame.Stop != icetray.I3Frame.Physics:
    raise RuntimeError("frame dumps/loads test failed - frame stop changed")

if len(frame) != 1:
    raise RuntimeError("frame dumps/loads test failed - number of frame objects changed")

if "test_01" not in frame:
    raise RuntimeError("frame dumps/loads test failed - frame object not in frame")

if frame["test_01"].value != 42:
    raise RuntimeError("frame dumps/loads test failed - frame object content changed")
