#!/usr/bin/env python3

import os
import unittest
from icecube import dataio, dataclasses
from I3Tray import *

theFileName = "TheFile.i3.gz"
testPos = dataclasses.I3Position(1,2,3)
testDir = dataclasses.I3Direction(-1,0,0)
testName = "Marty McFly"

class TestI3ScintGeo(unittest.TestCase):
    
    def test_I3ScintGeo_equality(self):
        dir = dataclasses.I3Direction(-1,0,0)
        pos = dataclasses.I3Position(1,2,3)
        
        ScintGeo1 = dataclasses.I3ScintGeo()
        ScintGeo1.position = testPos
        ScintGeo1.orientation = dataclasses.I3Orientation(testDir)
        ScintGeo1.scintName = testName
        
        ScintGeo2 = dataclasses.I3ScintGeo()
        ScintGeo2.position = testPos
        ScintGeo2.orientation = dataclasses.I3Orientation(testDir)
        ScintGeo2.scintName = testName
        
        self.assertEqual(ScintGeo1, ScintGeo2, "these should be the same.")
    
    def test_I3ScintGeo_equality_default_constructed(self):
        ScintGeo1 = dataclasses.I3ScintGeo()
        ScintGeo2 = dataclasses.I3ScintGeo()
        self.assertEqual(ScintGeo1, ScintGeo2, "these should be the same.")


    def test_Serialize(self):

        scintgeo = dataclasses.I3ScintGeo()
        scintgeo.position = testPos
        scintgeo.orientation = dataclasses.I3Orientation(testDir)
        scintgeo.scintName = testName

        theMap = dataclasses.I3ScintGeoMap()
        theMap[dataclasses.ScintKey(1,1)] = scintgeo

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

        self.assertEqual(readIn, scintgeo)
        newI3File.close()

    def tearDown(self):
        if os.path.exists(theFileName):
            os.remove(theFileName)
        self.assertEqual(os.path.exists(theFileName), False, "The file did not get removed in the cleanup!")




unittest.main()        
