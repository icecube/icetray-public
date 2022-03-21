#!/usr/bin/env python3

import os
import unittest
from icecube import dataio, dataclasses
from I3Tray import *

theFileName = "TheFile.i3.gz"

class TestI3IceActGeo(unittest.TestCase):
    
    def test_I3IceActGeo_equality(self):
        up    = dataclasses.I3Direction(0,0,1)
        d_dir = dataclasses.I3Direction(-1,0,0)
        pos   = dataclasses.I3Position(1,2,3)
        
        iceactGeo1 = dataclasses.I3IceActGeo()
        iceactGeo1.position = pos
        iceactGeo1.orientation = dataclasses.I3Orientation(d_dir, up)
        iceactGeo1.iceactName = 'Roof'
        iceactGeo1.numberofPixels = 64
        
        iceactGeo2 = dataclasses.I3IceActGeo()
        iceactGeo2.position = pos
        iceactGeo2.orientation = dataclasses.I3Orientation(d_dir, up)
        iceactGeo2.iceactName = 'Roof'
        iceactGeo2.numberofPixels = 64
        
        self.assertEqual(iceactGeo1, iceactGeo2, "these should be the same.")
    
    def test_I3IceActGeo_equality_default_constructed(self):
        iceactGeo1 = dataclasses.I3IceActGeo()
        iceactGeo2 = dataclasses.I3IceActGeo()
        self.assertEqual(iceactGeo1, iceactGeo2, "these should be the same.")


    def test_Serialize(self):

        iceactGeo = dataclasses.I3IceActGeo()
        iceactGeo.position = dataclasses.I3Position(1,2,3)
        iceactGeo.orientation = dataclasses.I3Orientation(dataclasses.I3Direction(0,0,1))
        iceactGeo.iceactName = 'Roof'
        iceactGeo.numberofPixels = 64

        theMap = dataclasses.I3IceActGeoMap()
        theMap[dataclasses.IceActKey(1,1)] = iceactGeo

        #Put into a file
        frameName = "FrameEntry"
        theI3File = dataio.I3File(theFileName, 'w')
        frame = icetray.I3Frame(icetray.I3Frame.Geometry)
        frame[frameName] = theMap
        theI3File.push(frame)
        theI3File.close()

        #Open the new file and assert that it is equal
        newI3File = dataio.I3File(theFileName, 'r')
        testFrame = newI3File.pop_frame()
        theMap = testFrame[frameName]
        readIn = theMap[theMap.keys()[0]]

        self.assertEqual(readIn, iceactGeo)
        newI3File.close()

    def tearDown(self):
        if os.path.exists(theFileName):
            os.remove(theFileName)
        self.assertEqual(os.path.exists(theFileName), False, "The file did not get removed in the cleanup!")


unittest.main()        
