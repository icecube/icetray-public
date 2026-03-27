#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube.dataclasses import I3PMTCal, I3PMTCalMap, SPEChargeDistribution
from icecube.icetray import OMKey, I3Frame

class TestI3PMTCal(unittest.TestCase):

    def test_I3PMTCal_equality_default_ctor(self):
        pc1 = I3PMTCal()
        pc2 = I3PMTCal()
        self.assertEqual(pc1, pc2, "these should be the same.")


    def test_I3PMTCal_string(self):
        pc1 = I3PMTCal()
        pc2 = I3PMTCal()
        self.assertEqual(pc1.__str__(), pc2.__str__(), "these should be the same.")


    def test_I3PMTCal_different_SPEdists(self):
        pc1 = I3PMTCal()
        pc2 = I3PMTCal()

        # populate the first with 2 exponential PDFs and a gaussian PDF
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Exponential(amplitude=0.2, width=3))
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Exponential(amplitude=0.5, width=0.7))
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Gaussian(amplitude=0.3, mean=1, sigma=0.2))

        # populate the second with 3 gaussian PDFs
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Gaussian(amplitude=0.2, mean=0, sigma=3))
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Gaussian(amplitude=0.5, mean=2, sigma=0.7))
        pc1.spe_charge_dist.pdfs.append(SPEChargeDistribution.Gaussian(amplitude=0.3, mean=1, sigma=0.2))

        self.frame = I3Frame(I3Frame.Calibration)

        # add an instance of I3PMTCalMap to the frame with the two different SPEChargeDistribution definitions
        pmt_cal_dict = {OMKey(88, 11, 1): pc1, # an "Upgrade function"
                        OMKey(35, 11, 0): pc2} # a "Gen1 function"
        self.frame['I3PMTCalMap'] = I3PMTCalMap(pmt_cal_dict)

        # obtain the I3PMTCalMap and make sure the distributions do not conflict
        pmt_cal_map = self.frame['I3PMTCalMap']
        self.assertEqual(pmt_cal_map[OMKey(88, 11, 1)], pc1)
        self.assertEqual(pmt_cal_map[OMKey(35, 11, 0)], pc2)
        
        

    def test_I3PMTCal_template(self):

        template = I3PMTCal()

        ### populate with non-constructor values
        template.relative_pmt_eff = 0.25
        template.noise_rate = 25
        template.spe_charge_dist.pdfs.append(SPEChargeDistribution.Exponential(amplitude=0.2, width=3))
        template.spe_charge_dist.pdfs.append(SPEChargeDistribution.Exponential(amplitude=0.5, width=0.7))
        template.spe_charge_dist.pdfs.append(SPEChargeDistribution.Gaussian(amplitude=0.3, mean=1, sigma=0.2))

        ### test the new template
        self.assertEqual(template.relative_pmt_eff, 0.25)
        self.assertEqual(template.noise_rate, 25)
        self.assertEqual(template.spe_charge_dist.pdfs[0], SPEChargeDistribution.Exponential(amplitude=0.2, width=3))
        self.assertEqual(template.spe_charge_dist.pdfs[1], SPEChargeDistribution.Exponential(amplitude=0.5, width=0.7))
        self.assertEqual(template.spe_charge_dist.pdfs[2], SPEChargeDistribution.Gaussian(amplitude=0.3, mean=1, sigma=0.2))

        self.frame = I3Frame(I3Frame.Calibration)

        ### populate a dictionary for a PMT
        pmt_cal_dict = {}
        pmt_cal_dict[OMKey(1, 2, 3)] = template

        self.frame['I3PMTCalMap'] = I3PMTCalMap(pmt_cal_dict)

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

        ### test SPE Charge Distribution
        self.assertEqual(pc.spe_charge_dist.pdfs[0], SPEChargeDistribution.Exponential(amplitude=0.2, width=3))
        self.assertEqual(pc.spe_charge_dist.pdfs[1], SPEChargeDistribution.Exponential(amplitude=0.5, width=0.7))
        self.assertEqual(pc.spe_charge_dist.pdfs[2], SPEChargeDistribution.Gaussian(amplitude=0.3, mean=1, sigma=0.2))
        pc.spe_charge_dist.pdfs[0].amplitude = 0.1
        pc.spe_charge_dist.pdfs[0].width = 2
        pc.spe_charge_dist.pdfs[1].amplitude = 0.4
        pc.spe_charge_dist.pdfs[1].width = 1.2
        pc.spe_charge_dist.pdfs[2].amplitude = 0.5
        pc.spe_charge_dist.pdfs[2].mean = 3.4
        pc.spe_charge_dist.pdfs[2].sigma = 0.6
        self.assertEqual(pc.spe_charge_dist.pdfs[0], SPEChargeDistribution.Exponential(amplitude=0.1, width=2))
        self.assertEqual(pc.spe_charge_dist.pdfs[1], SPEChargeDistribution.Exponential(amplitude=0.4, width=1.2))
        self.assertEqual(pc.spe_charge_dist.pdfs[2], SPEChargeDistribution.Gaussian(amplitude=0.5, mean=3.4, sigma=0.6))
        


unittest.main()
