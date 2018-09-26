#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3Orientation(unittest.TestCase):

    def test_I3Orientation_setOrientation(self):


        # create I3Orientation instance
        orientationInstance = dataclasses.I3Orientation()

        # create 3 vectors
        v1 = [1,0,0]
        v2 = [0,1,0]   # perpendicular to v1
        v3 = [2,0,0]   # not perpendicular to v1

        orientationInstance.set_orientation(v1[0],v1[1],v1[2],v2[0],v2[1],v2[2])     # this is supposed to work

        try:
            orientationInstance.set_orientation(v1[0],v1[1],v1[2],v3[0],v3[1],v3[2])     # this is supposed to chrash
            raise ValueError('I3Orientation::SetOrientation(v1,v2) is supposed to throw a RuntimeError if v1 and v2 are not perpendicular, but it does not!')
        except RuntimeError:
            pass

unittest.main()        
