#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, icetray


class TestI3mDOMCal(unittest.TestCase):

    def test_I3mDOMCal_equality_default_ctor(self):
        mc1 = dataclasses.I3mDOMCal()
        mc2 = dataclasses.I3mDOMCal()
        self.assertEqual(mc1, mc2, "these should be the same.")


    def test_I3mDOMCal_string(self):
        mc1 = dataclasses.I3mDOMCal()
        mc2 = dataclasses.I3mDOMCal()

        self.assertEqual(mc1.__str__(), mc2.__str__(), "these should be the same.")



    def test_I3mDOMCal_template(self):

        template = dataclasses.I3mDOMCal()

        ### populate with non-constructor values
        template.linearity_params.p0 = 700 # [PE]
        template.linearity_params.p1 = 175 # [PE]
        template.linearity_params.p2 = 300 # [PE]
        template.pmt_transit_time = 44         # [ns]
        template.pmt_transit_time_spread = 2.5 # [ns]
        template.hv_gain_relation.slope = 6.85      # [log(Gain) / log(V)]
        template.hv_gain_relation.intercept = -14.5 # [log(Gain)]
        

        ### test the new template
        self.assertEqual(template.adcToVolts, 0.045e-3)
        self.assertEqual(template.sampleRate, 120)
        self.assertEqual(template.frontEndImpedance, 75.35)
        #self.assertEqual(template.mdomTimeOffset, )
        self.assertEqual(template.discSampleRate, 960)
        self.assertEqual(template.linearity_params.p0, 700)
        self.assertEqual(template.linearity_params.p1, 175)
        self.assertEqual(template.linearity_params.p2, 300)
        self.assertEqual(template.pmt_transit_time, 44)
        self.assertEqual(template.pmt_transit_time_spread, 2.5)
        self.assertEqual(template.hv_gain_relation.slope, 6.85)
        self.assertEqual(template.hv_gain_relation.intercept, -14.5)

        self.frame = icetray.I3Frame(icetray.I3Frame.Calibration)

        ### populate a dictionary for all PMTs on one mDOM module
        mdom_cal_dict = {}
        for pmt in range(24):
            mdom_cal_dict[icetray.OMKey(90, 65, pmt)] = template

        self.frame['I3mDOMCalMap'] = dataclasses.I3mDOMCalMap(mdom_cal_dict)

        ### obtain the I3Map from the frame
        mdom_cal_map = self.frame['I3mDOMCalMap']
        self.assertEqual(len(mdom_cal_map), 24)

        ### test the last OMKey in the Map 
        last_omkey = list(mdom_cal_map.keys())[-1]
        self.assertEqual(last_omkey.string, 90)
        self.assertEqual(last_omkey.om, 65)
        self.assertEqual(last_omkey.pmt, 23)

        ### test the constant values
        mc = list(mdom_cal_map.values())[0]
        self.assertEqual(mc.adcToVolts, 0.045e-3)
        self.assertEqual(mc.sampleRate, 120)
        self.assertEqual(mc.frontEndImpedance, 75.35)
        #self.assertEqual(mc.mdomTimeOffset, )
        self.assertEqual(mc.discSampleRate, 960)


        ### test linearity params
        self.assertEqual(mc.linearity_params.p0, 700)
        self.assertEqual(mc.linearity_params.p1, 175)
        self.assertEqual(mc.linearity_params.p2, 300)
        mc.linearity_params.p0 = 24.56
        mc.linearity_params.p1 = 2.45
        mc.linearity_params.p2 = 0.12
        self.assertEqual(mc.linearity_params.p0, 24.56)
        self.assertEqual(mc.linearity_params.p1, 2.45)
        self.assertEqual(mc.linearity_params.p2, 0.12)

        ### test pmt transit time
        self.assertEqual(mc.pmt_transit_time, 44)
        mc.pmt_transit_time = 56.
        self.assertEqual(mc.pmt_transit_time, 56.)

        ### test pmt transit time spread
        self.assertEqual(mc.pmt_transit_time_spread, 2.5)
        mc.pmt_transit_time_spread = 3.45
        self.assertEqual(mc.pmt_transit_time_spread, 3.45)

        ### test hv gain relation
        self.assertEqual(mc.hv_gain_relation.slope, 6.85)
        self.assertEqual(mc.hv_gain_relation.intercept, -14.5)
        mc.hv_gain_relation.slope = 5.67
        mc.hv_gain_relation.intercept = -23.
        self.assertEqual(mc.hv_gain_relation.slope, 5.67)
        self.assertEqual(mc.hv_gain_relation.intercept, -23.)
        


unittest.main()
