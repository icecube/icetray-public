#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, icetray


class TestI3DEggStatus(unittest.TestCase):

    def test_I3DEggStatus_equality_default_ctor(self):
        ds1 = dataclasses.I3DEggStatus()
        ds2 = dataclasses.I3DEggStatus()
        self.assertEqual(ds1, ds2, "these should be the same.")


    def test_I3DEggStatus_string(self):
        ds1 = dataclasses.I3DEggStatus()
        ds2 = dataclasses.I3DEggStatus()

        self.assertEqual(ds1.__str__(), ds2.__str__(), "these should be the same.")


    def test_I3DEggStatus_template(self):

        template = dataclasses.I3DEggStatus()

        ### populate with non-constructor values
        template.trig_mode = dataclasses.I3DEggStatus.FIR
        template.trig_threshold = 12 # [Counts]
        template.fir_coefficients = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0]
        template.pre_samples = 16
        template.post_samples = 16
        template.pmt_hv = 1600 # [V]
        template.baseline_dac = 30000 # [DAC]
        
        ### test the new template
        self.assertEqual(template.enabled, True)
        self.assertEqual(template.trig_mode, dataclasses.I3DEggStatus.FIR)
        self.assertEqual(template.trig_threshold, 12)
        self.assertEqual(template.fir_coefficients, [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0])
        self.assertEqual(template.pre_samples, 16)
        self.assertEqual(template.post_samples, 16)
        self.assertEqual(template.pmt_hv, 1600)
        self.assertEqual(template.baseline_dac, 30000)

        self.frame = icetray.I3Frame(icetray.I3Frame.DetectorStatus)

        ### populate a dictionary for all PMTs on one DEgg module
        degg_status_dict = {}
        for pmt in range(2):
            degg_status_dict[icetray.OMKey(90, 64, pmt)] = template

        self.frame['I3DEggStatusMap'] = dataclasses.I3DEggStatusMap(degg_status_dict)

        ### obtain the I3Map from the frame
        degg_status_map = self.frame['I3DEggStatusMap']
        self.assertEqual(len(degg_status_map), 2)

        ### test the last OMKey in the Map 
        last_omkey = list(degg_status_map.keys())[-1]
        self.assertEqual(last_omkey.string, 90)
        self.assertEqual(last_omkey.om, 64)
        self.assertEqual(last_omkey.pmt, 1)

        ds = list(degg_status_map.values())[0]

        ### test enabled
        self.assertEqual(ds.enabled, True)
        ds.enabled = False
        self.assertEqual(ds.enabled, False)

        ### test trigger mode
        self.assertEqual(ds.trig_mode, dataclasses.I3DEggStatus.FIR)
        ds.trig_mode = dataclasses.I3DEggStatus.ADC
        self.assertEqual(ds.trig_mode, dataclasses.I3DEggStatus.ADC)

        ### test trigger threshold
        self.assertEqual(ds.trig_threshold, 12)
        ds.trig_threshold = 34
        self.assertEqual(ds.trig_threshold, 34)

        ### test FIR coefficients
        self.assertEqual(ds.fir_coefficients, [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0])
        ds.fir_coefficients = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
        self.assertEqual(ds.fir_coefficients, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])

        ### test pre samples
        self.assertEqual(ds.pre_samples, 16)
        ds.pre_samples = 23
        self.assertEqual(ds.pre_samples, 23)

        ### test post samples
        self.assertEqual(ds.post_samples, 16)
        ds.post_samples = 45
        self.assertEqual(ds.post_samples, 45)

        ### test pmt hv
        self.assertEqual(ds.pmt_hv, 1600)
        ds.pmt_hv = 1234
        self.assertEqual(ds.pmt_hv, 1234)

        ### test baseline dac
        self.assertEqual(ds.baseline_dac, 30000)
        ds.baseline_dac = 23456
        self.assertEqual(ds.baseline_dac, 23456)



unittest.main()
