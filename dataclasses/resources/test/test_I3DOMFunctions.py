#!/usr/bin/env python

import unittest
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

        mspe1 = dataclasses.mean_spe_charge(ds1,dc1)
        mspe2 = dataclasses.mean_spe_charge(ds2,dc2)

        self.assertEqual(mspe1, mspe2, "these should be the same.")


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

