#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
from icecube import icetray, dataclasses, phys_services
from icecube.icetray import I3Units, logging

icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)


class PhysServicesPybindings(unittest.TestCase):
    def testI3SMLOMKeyToMBID(self):
        logging.log_debug("Working out I3XMLOMKey2MBID service")

        infile = './phys-services/resources/mainboard_ids.xml.gz'
        ms = phys_services.I3XMLOMKey2MBID(infile)
        ambid = ms.get_mbid(icetray.OMKey(2, 2))
        aom = ms.get_omkey(ambid)
        self.assertEqual(aom, icetray.OMKey(2, 2), "Failed to find the right om in I3XMLOMKey2MBID")

    def testI3Calculator(self):
        logging.log_debug("Working out I3Calculator")

        part1 = dataclasses.I3Particle()
        part1.dir = dataclasses.I3Direction(1.0, 0.0, 0.0)
        part2 = dataclasses.I3Particle()
        part2.dir = dataclasses.I3Direction(0.0, 1.0, 0.0)
        anglediff = phys_services.I3Calculator.angle(part1, part2)

        logging.log_debug("Particle 1: %s" % part1.dir)
        logging.log_debug("Particle 2: %s" % part2.dir)
        logging.log_debug("Angle diff: %.2f degrees" % (anglediff / I3Units.deg))

        self.assertEqual(part1.dir.azimuth / I3Units.deg, 180, "Particle 1 azimuth should be 180")
        self.assertEqual(part2.dir.azimuth / I3Units.deg, 270, "Particle 2 azimuth should be 270")
        self.assertEqual(anglediff / I3Units.deg, 90, "Angle difference between Particle 1 & 2 should be 90")

    # FIXME: this is a silly test
    def testI3CutValues(self):
        logging.log_debug("Working out I3CutValues")

        ## This is just a dumb container.
        cv = phys_services.I3CutValues()
        cog_pos = dataclasses.I3Position(0.0, 0.0, 0.0)
        cv.cog = cog_pos
        cv.ldir = 500.5 * I3Units.m
        cv.ndir = 5
        cv.nchan = 55
        logging.log_debug("Your I3CutValues: %s %.2f %d %d" % (cv.cog, cv.ldir, cv.ndir, cv.nchan))
        self.assertEqual(cv.ldir, 500.5 * I3Units.m, "Should have gotten out what we put in")
        self.assertEqual(cv.ndir, 5, "Should have gotten out what we put in")
        self.assertEqual(cv.nchan, 55, "Should have gotten out what we put in")

    def testI3GCDFileCalibrationService(self):
        logging.log_debug("Working out GCDFileService")

        mytime = dataclasses.I3Time(2010, 158082172000000000)
        infile = os.path.expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")
        my_fs = phys_services.I3GCDFileCalibrationService(infile)
        my_cal = my_fs.get_calibration(mytime)
        logging.log_debug("I found calibrations for %d DOMs" % len(my_cal.dom_cal))
        self.assertTrue(len(my_cal.dom_cal) == 5433, "Should have gotten 5433 dom cals")

    def testI3GSLRandomService(self):
        logging.log_debug("Working out randoms")

        rng = phys_services.I3GSLRandomService(31334)
        r = [rng.gaus(0, 1) for x in range(100)]
        logging.log_debug("Please take these 100 rands: \n %s" % r)
        self.assertTrue(len(r) == 100, "Should have gotten 100 rands")


if __name__ == '__main__':
    unittest.main()
