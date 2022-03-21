#!/usr/bin/env python3

import unittest
import math
from icecube import dataclasses
from icecube.dataclasses import I3DOMCalibration
from icecube.dataclasses import I3DOMStatus

class TestI3DOMFunctions(unittest.TestCase):

    def test_MeanSPECharge_equality(self):
        fit1 = dataclasses.LinearFit()
        fit2 = dataclasses.LinearFit()
        ds1  = dataclasses.I3DOMStatus()
        dc1  = dataclasses.I3DOMCalibration()
        ds2  = dataclasses.I3DOMStatus()
        dc2  = dataclasses.I3DOMCalibration()

        fit1.slope       = 1.618
        fit2.slope       = 1.618
        fit1.intercept   = 1.20205
        fit2.intercept   = 1.20205
        ds1.dac_fadc_ref = 0.1234
        ds2.dac_fadc_ref = 0.1234

        dc1.fadc_baseline_fit = fit1
        dc2.fadc_baseline_fit = fit2

        mspe1 = dataclasses.mean_spe_charge(dc1)
        mspe2 = dataclasses.mean_spe_charge(dc2)

        self.assertEqual(mspe1, mspe2, "these should be the same.")

        d1 = dataclasses.SPEChargeDistribution()
        d1.exp1_amp   = 6.68282
        d1.exp1_width = 0.0342546
        d1.exp2_amp   = 0.521208
        d1.exp2_width = 0.445405
        d1.gaus_amp   = 0.688097
        d1.slc_gaus_mean  = 1.
        d1.gaus_mean  = 1.
        d1.gaus_width = 0.312677
        d1.compensation_factor = 1.3

        dc1.combined_spe_charge_distribution = d1
        
        d2 = dataclasses.SPEChargeDistribution()
        d2.exp1_amp   = 6.68282
        d2.exp1_width = 0.0342546
        d2.exp2_amp   = 0.521208
        d2.exp2_width = 0.445405
        d2.gaus_amp   = 0.688097
        d2.gaus_mean  = 1.
        d2.slc_gaus_mean  = 1.
        d2.gaus_width = 0.312677
        d2.compensation_factor = 1.3        

        dc2.combined_spe_charge_distribution = d2

        mspe1 = dataclasses.mean_spe_charge(dc1)
        mspe2 = dataclasses.mean_spe_charge(dc2)

        self.assertEqual(mspe1, mspe2, "these should be the same.")

        manalytic = d1.compensation_factor*(
            d1.exp1_amp*d1.exp1_width**2+d1.exp2_amp*d1.exp2_width**2
            +d1.gaus_amp*math.sqrt(math.pi/2)*d1.gaus_mean*d1.gaus_width*
            (1+math.erf(d1.gaus_mean/(d1.gaus_width*math.sqrt(2))))
            +d1.gaus_amp*d1.gaus_width**2*math.exp(-0.5*(d1.gaus_mean/d1.gaus_width)**2))
        self.assertTrue(manalytic-mspe1<0.01, "these should be close.")
        

    def test_FADCBaseline_equality(self):
        fit1 = dataclasses.LinearFit()
        fit2 = dataclasses.LinearFit()
        ds1  = dataclasses.I3DOMStatus()
        dc1  = dataclasses.I3DOMCalibration()
        ds2  = dataclasses.I3DOMStatus()
        dc2  = dataclasses.I3DOMCalibration()

        fit1.slope       = 1.618
        fit2.slope       = 1.618
        fit1.intercept   = 1.20205
        fit2.intercept   = 1.20205
        ds1.dac_fadc_ref = 0.1234
        ds2.dac_fadc_ref = 0.1234

        dc1.fadc_baseline_fit = fit1
        dc2.fadc_baseline_fit = fit2

        fadcb1 = dataclasses.fadc_baseline(ds1,dc1)
        fadcb2 = dataclasses.fadc_baseline(ds2,dc2)

        self.assertEqual(fadcb1, fadcb2, "these should be the same.")


unittest.main()        

