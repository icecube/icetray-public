#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, icetray

class TestI3PMTCal(unittest.TestCase):

    def test_I3PMTCal_equality_default_ctor(self):
        pc1 = dataclasses.I3PMTCal()
        pc2 = dataclasses.I3PMTCal()
        self.assertEqual(pc1, pc2, "these should be the same.")


    def test_I3PMTCal_string(self):
        pc1 = dataclasses.I3PMTCal()
        pc2 = dataclasses.I3PMTCal()

        self.assertEqual(pc1.__str__(), pc2.__str__(), "these should be the same.")


    def test_I3PMTCal_template(self):

        template = dataclasses.I3PMTCal()

        ### populate with non-constructor values
        template.relative_pmt_eff = 0.25
        template.noise_rate = 25

        ### test the new template
        self.assertEqual(template.relative_pmt_eff, 0.25)
        self.assertEqual(template.noise_rate, 25)

        self.frame = icetray.I3Frame(icetray.I3Frame.Calibration)

        ### populate a dictionary for a PMT
        pmt_cal_dict = {}
        pmt_cal_dict[icetray.OMKey(1, 2, 3)] = template

        self.frame['I3PMTCalMap'] = dataclasses.I3PMTCalMap(pmt_cal_dict)

        ### obtain the I3Map from the frame
        pmt_cal_map = self.frame['I3PMTCalMap']
        self.assertEqual(len(pmt_cal_map), 1)

        ### test the last OMKey in the Map 
        last_omkey = list(pmt_cal_map.keys())[-1]
        self.assertEqual(last_omkey.string, 1)
        self.assertEqual(last_omkey.om, 2)
        self.assertEqual(last_omkey.pmt, 3)

        pc = list(pmt_cal_map.values())[0]

        ### test relative PMT efficiency
        self.assertEqual(pc.relative_pmt_eff, 0.25)
        pc.relative_pmt_eff = 0.30
        self.assertEqual(pc.relative_pmt_eff, 0.30)

        ### test noise rate
        self.assertEqual(pc.noise_rate, 25)
        pc.noise_rate = 34.
        self.assertEqual(pc.noise_rate, 34.)

        


unittest.main()
