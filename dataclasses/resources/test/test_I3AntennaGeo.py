#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from tempfile import NamedTemporaryFile

from icecube import dataclasses, dataio, icetray


class I3AntennaGeo(unittest.TestCase):
    def test_Serialize(self):
        frameName = "FrameEntry"
        testPos = dataclasses.I3Position(1, 2, 3)
        testOrient = dataclasses.I3Orientation(0.0, 1.0, 0.0, 1.0, 0.0, 0.0)
        testHeight = 1234.5
        testCable = 7.7
        testName = "Marty McFly"
        testType = dataclasses.I3AntennaGeo.AntennaType.SKALA2

        antgeo = dataclasses.I3AntennaGeo()
        antgeo.position = testPos
        antgeo.orientation = testOrient
        antgeo.heightAboveSnow = testHeight
        antgeo.cableLength = testCable
        antgeo.antennaName = testName
        antgeo.antennatype = testType

        theMap = dataclasses.I3AntennaGeoMap()
        theMap[dataclasses.AntennaKey(1, 1)] = antgeo

        with NamedTemporaryFile() as t:
            theFileName = t.name

            # Put into a file
            theI3File = dataio.I3File(theFileName, "w")
            frame = icetray.I3Frame(icetray.I3Frame.Geometry)
            frame[frameName] = theMap
            theI3File.push(frame)
            theI3File.close()

            # Open the new file and assert that it is equal
            newI3File = dataio.I3File(theFileName, "r")
            testFrame = newI3File.pop_frame()
            newI3File.close()

        theMap = testFrame[frameName]
        readIn = theMap[next(iter(theMap.keys()))]

        self.assertEqual(readIn, antgeo)


if __name__ == "__main__":
    unittest.main()
