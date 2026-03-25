#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, icetray
from icecube.icetray import I3Frame, I3Units, OMKey


class TestI3DEggCal(unittest.TestCase):

    def test_LinearityParameters_equality(self):
        params1 = dataclasses.LinearityParameters()
        params1.p0 = 1.618
        params1.p1 = 1.645
        params1.p2 = 1.20205

        params2 = dataclasses.LinearityParameters()
        params2.p0 = 1.618
        params2.p1 = 1.645
        params2.p2 = 1.20205

        self.assertEqual(params1, params2, "these should be the same.")

        params2.p0 = 0.
        self.assertNotEqual(params1, params2, "these should be the same.")



    def test_I3DEggCal_equality_default_ctor(self):
        dc1 = dataclasses.I3DEggCal()
        dc2 = dataclasses.I3DEggCal()
        self.assertEqual(dc1, dc2, "these should be the same.")



    def test_LinearityParameters_equality_default_ctor(self):
        o1 = dataclasses.LinearityParameters()
        o2 = dataclasses.LinearityParameters()
        self.assertEqual(o1, o2, "these should be the same.")



    def test_LinearityParameters_string(self):
        params1 = dataclasses.LinearityParameters()
        params1.p0 = 1.618
        params1.p1 = 1.645
        params1.p2 = 1.20205
        params2 = dataclasses.LinearityParameters()
        params2.p0 = 1.618
        params2.p1 = 1.645
        params2.p2 = 1.20205

        self.assertEqual(params1.__str__(), params2.__str__(), "these should be the same.")



    def test_I3DEggCal_string(self):
        dc1 = dataclasses.I3DEggCal()
        dc2 = dataclasses.I3DEggCal()

        self.assertEqual(dc1.__str__(), dc2.__str__(), "these should be the same.")



    def test_I3DEggCal_template(self):

        template = dataclasses.I3DEggCal()

        ### populate with non-constructor values
        template.temperature = (-40 + 273.0) * I3Units.kelvin # (-40 degrees Celsius)
        template.linearity_params.p0 = 36.71 * I3Units.mA
        template.linearity_params.p1 = 1.89 * I3Units.mA
        template.linearity_params.p2 = 0.14 * I3Units.mA
        template.pmt_transit_time = 58 * I3Units.ns
        template.pmt_transit_time_spread = 2.89 * I3Units.ns
        template.hv_gain_relation.slope = 6.25    # [log(Gain) / log(V)]
        template.hv_gain_relation.intercept = -30 # [log(Gain)]
        template.dac_baseline_relation.slope = 4/15  # [Counts / DAC]
        template.dac_baseline_relation.intercept = 0 # [Counts]
        template.tau_params.p0 = 4.9 * I3Units.microsecond
        template.tau_params.p1 = 26.7 * I3Units.microsecond
        template.tau_params.p2 = (28.3 + 273.0) * I3Units.kelvin # (28.3 degrees Celsius)

        ### test the new template
        self.assertEqual(template.adcToVolts, 7.280e-5 * I3Units.V)
        self.assertEqual(template.sampleRate, 240 * I3Units.megahertz)
        self.assertEqual(template.frontEndImpedance, 36.96 * I3Units.ohm)
        #self.assertEqual(template.deggTimeOffset, ) current constant value is set to NAN, once measurement is made and the constant is updated then uncomment and add this condition
        self.assertEqual(template.temperature, (-40 + 273.0) * I3Units.kelvin)
        self.assertEqual(template.linearity_params.p0, 36.71 * I3Units.mA)
        self.assertEqual(template.linearity_params.p1, 1.89 * I3Units.mA)
        self.assertEqual(template.linearity_params.p2, 0.14 * I3Units.mA)
        self.assertEqual(template.pmt_transit_time, 58 * I3Units.ns)
        self.assertEqual(template.pmt_transit_time_spread, 2.89 * I3Units.ns)
        self.assertEqual(template.hv_gain_relation.slope, 6.25)
        self.assertEqual(template.hv_gain_relation.intercept, -30)
        self.assertEqual(template.dac_baseline_relation.slope, 4/15)
        self.assertEqual(template.dac_baseline_relation.intercept, 0)
        self.assertEqual(template.tau_params.p0, 4.9 * I3Units.microsecond)
        self.assertEqual(template.tau_params.p1, 26.7 * I3Units.microsecond)
        self.assertEqual(template.tau_params.p2, (28.3 + 273.0) * I3Units.kelvin)

        self.frame = I3Frame(I3Frame.Calibration)

        ### populate a dictionary for all PMTs on one DEgg module
        degg_cal_dict = {}
        for pmt in range(2):
            degg_cal_dict[OMKey(90, 64, pmt)] = template

        self.frame['I3DEggCalMap'] = dataclasses.I3DEggCalMap(degg_cal_dict)

        ### obtain the I3Map from the frame
        degg_cal_map = self.frame['I3DEggCalMap']
        self.assertEqual(len(degg_cal_map), 2)

        ### test the last OMKey in the Map 
        last_omkey = list(degg_cal_map.keys())[-1]
        self.assertEqual(last_omkey.string, 90)
        self.assertEqual(last_omkey.om, 64)
        self.assertEqual(last_omkey.pmt, 1)

        ### test the constant values
        dc = list(degg_cal_map.values())[0]
        self.assertEqual(dc.adcToVolts, 7.280e-5 * I3Units.V)
        self.assertEqual(dc.sampleRate, 240 * I3Units.megahertz)
        self.assertEqual(dc.frontEndImpedance, 36.96 * I3Units.ohm)
        #self.assertEqual(dc.deggTimeOffset, ) current constant value is set to NAN, once measurement is made and the constant is updated then uncomment and add this condition

        ### test temperature
        self.assertEqual(dc.temperature, (-40 + 273.) * I3Units.kelvin)
        dc.temperature = (20. + 273.) * I3Units.kelvin
        self.assertEqual(dc.temperature, 293. * I3Units.kelvin)

        ### test linearity params
        self.assertEqual(dc.linearity_params.p0, 36.71 * I3Units.mA)
        self.assertEqual(dc.linearity_params.p1, 1.89 * I3Units.mA)
        self.assertEqual(dc.linearity_params.p2, 0.14 * I3Units.mA)
        dc.linearity_params.p0 = 24.56 * I3Units.mA
        dc.linearity_params.p1 = 2.45 * I3Units.mA
        dc.linearity_params.p2 = 0.12 * I3Units.mA
        self.assertEqual(dc.linearity_params.p0, 24.56 * I3Units.mA)
        self.assertEqual(dc.linearity_params.p1, 2.45 * I3Units.mA)
        self.assertEqual(dc.linearity_params.p2, 0.12 * I3Units.mA)

        ### test pmt transit time
        self.assertEqual(dc.pmt_transit_time, 58 * I3Units.ns)
        dc.pmt_transit_time = 56. * I3Units.ns
        self.assertEqual(dc.pmt_transit_time, 56. * I3Units.ns)

        ### test pmt transit time spread
        self.assertEqual(dc.pmt_transit_time_spread, 2.89 * I3Units.ns)
        dc.pmt_transit_time_spread = 3.45 * I3Units.ns
        self.assertEqual(dc.pmt_transit_time_spread, 3.45 * I3Units.ns)

        ### test hv gain relation
        self.assertEqual(dc.hv_gain_relation.slope, 6.25)
        self.assertEqual(dc.hv_gain_relation.intercept, -30)
        dc.hv_gain_relation.slope = 5.67
        dc.hv_gain_relation.intercept = -23.
        self.assertEqual(dc.hv_gain_relation.slope, 5.67)
        self.assertEqual(dc.hv_gain_relation.intercept, -23.)

        ### test dac baseline relation
        self.assertEqual(dc.dac_baseline_relation.slope, 4/15)
        self.assertEqual(dc.dac_baseline_relation.intercept, 0)
        dc.dac_baseline_relation.slope = 10/11
        dc.dac_baseline_relation.intercept = 1.
        self.assertEqual(dc.dac_baseline_relation.slope, 10/11)
        self.assertEqual(dc.dac_baseline_relation.intercept, 1.)

        ### test tau params
        self.assertEqual(dc.tau_params.p0, 4.9 * I3Units.microsecond)
        self.assertEqual(dc.tau_params.p1, 26.7 * I3Units.microsecond)
        self.assertEqual(dc.tau_params.p2, (28.3 + 273.) * I3Units.kelvin)
        dc.tau_params.p0 = 5.6 * I3Units.microsecond
        dc.tau_params.p1 = 23.4 * I3Units.microsecond
        dc.tau_params.p2 = (34.5 + 273.) * I3Units.kelvin
        self.assertEqual(dc.tau_params.p0, 5.6 * I3Units.microsecond)
        self.assertEqual(dc.tau_params.p1, 23.4 * I3Units.microsecond)
        self.assertEqual(dc.tau_params.p2, 307.5 * I3Units.kelvin)




unittest.main()
