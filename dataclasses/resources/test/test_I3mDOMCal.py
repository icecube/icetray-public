#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import math
import unittest
from icecube import dataclasses, icetray
from icecube.icetray import I3Frame, I3Units, OMKey


class TestI3mDOMCal(unittest.TestCase):

    def test_I3mDOMCal_equality_default_ctor(self):
        mc1 = dataclasses.I3mDOMCal()
        mc2 = dataclasses.I3mDOMCal()
        self.assertEqual(mc1, mc2, "these should be the same.")


    def test_I3mDOMCal_string(self):
        mc1 = dataclasses.I3mDOMCal()
        mc2 = dataclasses.I3mDOMCal()
        self.assertEqual(mc1.__str__(), mc2.__str__(), "these should be the same.")


    def test_I3mDOMCal_functions(self):
        mc = dataclasses.I3mDOMCal()
        
        # GetValidBaselineValue will return adc_baseline_value only if the given DAC matches adc_baseline_dac...
        mc.adc_baseline_value = 3600 # [Counts]
        mc.adc_baseline_dac = 13300  # [DAC]
        self.assertEqual(mc.GetValidBaselineValue(13300), 3600)
        with self.assertRaises(RuntimeError): mc.GetValidBaselineValue(13301) # ...otherwise it returns a RuntimeError

        # GetValidDiscThreshold will return disc_threshold only if the given DAC matches disc_dac...
        mc.disc_threshold = 0.002 * I3Units.volt
        mc.disc_dac = 23500          # [DAC]
        self.assertEqual(mc.GetValidDiscThreshold(23500), 0.002 * I3Units.volt)
        with self.assertRaises(RuntimeError): mc.GetValidDiscThreshold(23501) # ...otherwise it returns a RuntimeError



    def test_I3mDOMCal_template(self):

        template = dataclasses.I3mDOMCal()

        ### populate with non-constructor values
        template.linearity_params.p0 = 700 # [PE]
        template.linearity_params.p1 = 175 # [PE]
        template.linearity_params.p2 = 300 # [PE]
        template.pmt_transit_time = 44 * I3Units.ns         # [ns]
        template.pmt_transit_time_spread = 2.5 * I3Units.ns # [ns]
        template.hv_gain_relation.slope = 6.85      # [log(Gain) / log(V)]
        template.hv_gain_relation.intercept = -14.5 # [log(Gain)]
        template.adc_baseline_rms = 2.5    # [Counts]
        template.adc_baseline_value = 3600 # [Counts]
        template.adc_baseline_dac = 13300  # [DAC]
        template.disc_threshold = 0.002 * I3Units.volt
        template.disc_dac = 23500          # [DAC]
        

        ### test the new template
        self.assertEqual(template.adcToVolts, 0.045e-3 * I3Units.V)
        self.assertEqual(template.sampleRate, 120 * I3Units.megahertz)
        self.assertEqual(template.frontEndImpedance, 75.35 * I3Units.ohm)
        #self.assertEqual(template.mdomTimeOffset, ) current constant value is set to NAN, once measurement is made and the constant is updated then uncomment and add this condition
        self.assertEqual(template.discSampleRate, 960 * I3Units.megahertz)
        self.assertEqual(template.linearity_params.p0, 700)
        self.assertEqual(template.linearity_params.p1, 175)
        self.assertEqual(template.linearity_params.p2, 300)
        self.assertEqual(template.pmt_transit_time, 44 * I3Units.ns)
        self.assertEqual(template.pmt_transit_time_spread, 2.5 * I3Units.ns)
        self.assertEqual(template.hv_gain_relation.slope, 6.85)
        self.assertEqual(template.hv_gain_relation.intercept, -14.5)
        self.assertEqual(template.adc_baseline_rms, 2.5)
        self.assertEqual(template.adc_baseline_value, 3600)
        self.assertEqual(template.adc_baseline_dac, 13300) 
        self.assertEqual(template.disc_threshold, 0.002 * I3Units.volt)
        self.assertEqual(template.disc_dac, 23500)

        self.frame = I3Frame(I3Frame.Calibration)

        ### populate a dictionary for all PMTs on one mDOM module
        mdom_cal_dict = {}
        for pmt in range(24):
            mdom_cal_dict[OMKey(90, 65, pmt)] = template

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
        self.assertEqual(mc.adcToVolts, 0.045e-3 * I3Units.V)
        self.assertEqual(mc.sampleRate, 120 * I3Units.megahertz)
        self.assertEqual(mc.frontEndImpedance, 75.35 * I3Units.ohm)
        #self.assertEqual(mc.mdomTimeOffset, ) current constant value is set to NAN, once measurement is made and the constant is updated then uncomment and add this condition
        self.assertEqual(mc.discSampleRate, 960 * I3Units.megahertz)


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
        self.assertEqual(mc.pmt_transit_time, 44 * I3Units.ns)
        mc.pmt_transit_time = 56. * I3Units.ns
        self.assertEqual(mc.pmt_transit_time, 56. * I3Units.ns)

        ### test pmt transit time spread
        self.assertEqual(mc.pmt_transit_time_spread, 2.5 * I3Units.ns)
        mc.pmt_transit_time_spread = 3.45 * I3Units.ns
        self.assertEqual(mc.pmt_transit_time_spread, 3.45 * I3Units.ns)

        ### test hv gain relation
        self.assertEqual(mc.hv_gain_relation.slope, 6.85)
        self.assertEqual(mc.hv_gain_relation.intercept, -14.5)
        mc.hv_gain_relation.slope = 5.67
        mc.hv_gain_relation.intercept = -23.
        self.assertEqual(mc.hv_gain_relation.slope, 5.67)
        self.assertEqual(mc.hv_gain_relation.intercept, -23.)

        ### test ADC Baseline Uncertainty
        self.assertEqual(mc.adc_baseline_rms, 2.5)
        mc.adc_baseline_rms = 6.7
        self.assertEqual(mc.adc_baseline_rms, 6.7)

        ### test ADC Baseline DAC/Value pair
        self.assertEqual(mc.adc_baseline_value, 3600)
        self.assertEqual(mc.adc_baseline_dac, 13300)
        mc.adc_baseline_value = 2000
        mc.adc_baseline_dac = 9900
        self.assertEqual(mc.adc_baseline_value, 2000)
        self.assertEqual(mc.adc_baseline_dac, 9900)

        ### test Discriminator Threshold DAC/Value pair
        self.assertEqual(mc.disc_threshold, 0.002 * I3Units.volt)
        self.assertEqual(mc.disc_dac, 23500)
        mc.disc_threshold = 0.005 * I3Units.volt
        mc.disc_dac = 12300
        self.assertEqual(mc.disc_threshold, 0.005 * I3Units.volt)
        self.assertEqual(mc.disc_dac, 12300)
        


unittest.main()
