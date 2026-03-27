#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, icetray


class TestI3mDOMStatus(unittest.TestCase):

    def test_I3mDOMStatus_equality_default_ctor(self):
        ms1 = dataclasses.I3mDOMStatus()
        ms2 = dataclasses.I3mDOMStatus()
        self.assertEqual(ms1, ms2, "these should be the same.")


    def test_I3mDOMStatus_string(self):
        ms1 = dataclasses.I3mDOMStatus()
        ms2 = dataclasses.I3mDOMStatus()

        self.assertEqual(ms1.__str__(), ms2.__str__(), "these should be the same.")


    def test_I3mDOMStatus_template(self):

        template = dataclasses.I3mDOMStatus()

        ### populate with non-constructor values
        template.trig_mode = dataclasses.I3mDOMStatus.Disc
        template.adc_threshold = 0
        template.readout_mode = dataclasses.I3mDOMStatus.VARIABLE_LENGTH
        template.pre_samples = 16
        template.post_samples = 16
        template.pmt_hv = 85 # [V]
        template.adc_baseline_dac = 13500 # [DAC]
        template.disc_dac = 23610 # [DAC]
        
        ### test the new template
        self.assertEqual(template.enabled, True)
        self.assertEqual(template.trig_mode, dataclasses.I3mDOMStatus.Disc)
        self.assertEqual(template.adc_threshold, 0)
        self.assertEqual(template.readout_mode, dataclasses.I3mDOMStatus.VARIABLE_LENGTH)
        self.assertEqual(template.pre_samples, 16)
        self.assertEqual(template.post_samples, 16)
        self.assertEqual(template.pmt_hv, 85)
        self.assertEqual(template.adc_baseline_dac, 13500)
        self.assertEqual(template.disc_dac, 23610)

        self.frame = icetray.I3Frame(icetray.I3Frame.DetectorStatus)

        ### populate a dictionary for all PMTs on one mDOM module
        mdom_status_dict = {}
        for pmt in range(24):
            mdom_status_dict[icetray.OMKey(90, 65, pmt)] = template

        self.frame['I3mDOMStatusMap'] = dataclasses.I3mDOMStatusMap(mdom_status_dict)

        ### obtain the I3Map from the frame
        mdom_status_map = self.frame['I3mDOMStatusMap']
        self.assertEqual(len(mdom_status_map), 24)

        ### test the last OMKey in the Map 
        last_omkey = list(mdom_status_map.keys())[-1]
        self.assertEqual(last_omkey.string, 90)
        self.assertEqual(last_omkey.om, 65)
        self.assertEqual(last_omkey.pmt, 23)

        ms = list(mdom_status_map.values())[0]

        ### test enabled
        self.assertEqual(ms.enabled, True)
        ms.enabled = False
        self.assertEqual(ms.enabled, False)

        ### test trigger mode
        self.assertEqual(ms.trig_mode, dataclasses.I3mDOMStatus.Disc)
        ms.trig_mode = dataclasses.I3mDOMStatus.ADC
        self.assertEqual(ms.trig_mode, dataclasses.I3mDOMStatus.ADC)

        ### test adc threshold
        self.assertEqual(ms.adc_threshold, 0)
        ms.adc_threshold = 34
        self.assertEqual(ms.adc_threshold, 34)

        ### test readout mode
        self.assertEqual(ms.readout_mode, dataclasses.I3mDOMStatus.VARIABLE_LENGTH)
        ms.readout_mode = dataclasses.I3mDOMStatus.FIXED_LENGTH
        self.assertEqual(ms.readout_mode, dataclasses.I3mDOMStatus.FIXED_LENGTH)

        ### test pre samples
        self.assertEqual(ms.pre_samples, 16)
        ms.pre_samples = 23
        self.assertEqual(ms.pre_samples, 23)

        ### test post samples
        self.assertEqual(ms.post_samples, 16)
        ms.post_samples = 45
        self.assertEqual(ms.post_samples, 45)

        ### test pmt hv
        self.assertEqual(ms.pmt_hv, 85)
        ms.pmt_hv = 56
        self.assertEqual(ms.pmt_hv, 56)

        ### test adc baseline
        self.assertEqual(ms.adc_baseline_dac, 13500)
        ms.adc_baseline_dac = 23456
        self.assertEqual(ms.adc_baseline_dac, 23456)

        ### test disc dac
        self.assertEqual(ms.disc_dac, 23610)
        ms.disc_dac = 23456
        self.assertEqual(ms.disc_dac, 23456)




unittest.main()
