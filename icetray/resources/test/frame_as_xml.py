#!/usr/bin/env python

"""
Test the I3Frame's dumps xml ok
"""

from icecube import icetray
frame = icetray.I3Frame()
frame['int'] = icetray.I3Int(42)
print("%s" % frame.as_xml('int'))
