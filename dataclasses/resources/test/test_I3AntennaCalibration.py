#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from tempfile import NamedTemporaryFile

from icecube import dataclasses, dataio, icetray


class I3AntennaCal(unittest.TestCase):
    def test_Serialize(self):
        frameName = "FrameEntry"
        testAnt = dataclasses.I3AntennaCal.AntennaType.SKALA2
        testCable = dataclasses.I3AntennaCal.CableType.LMR400
        testDaq = dataclasses.I3AntennaCal.DaqType.Board1_Taxi3_0
        testLength = 7.7

        antcal = dataclasses.I3AntennaCal()
        antcal.antennaType = testAnt
        antcal.cableType = testCable
        antcal.DaqType = testDaq
        antcal.cableLength = testLength

        theMap = dataclasses.I3AntennaCalMap()
        theMap[dataclasses.AntennaKey(1, 1)] = antcal

        with NamedTemporaryFile() as t:
            theFileName = t.name

            # Put into a file
            theI3File = dataio.I3File(theFileName, "w")
            frame = icetray.I3Frame(icetray.I3Frame.Calibration)
            frame[frameName] = theMap
            theI3File.push(frame)
            theI3File.close()

            # Open the new file and assert that it is equal
            newI3File = dataio.I3File(theFileName, "r")
            testFrame = newI3File.pop_frame()
            newI3File.close()

        theMap = testFrame[frameName]
        readIn = theMap[list(theMap.keys())[0]]

        self.assertEqual(readIn, antcal)


if __name__ == "__main__":
    unittest.main()
