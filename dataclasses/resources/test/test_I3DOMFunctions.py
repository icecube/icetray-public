#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
import math
import numpy as np
from scipy import integrate
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

        Exp = dataclasses.SPEChargeDistribution.Exponential
        Gaus = dataclasses.SPEChargeDistribution.Gaussian

        d1 = dataclasses.SPEChargeDistribution()
        d1.pdfs.append( Exp(amplitude=6.68282,  width=0.0342546))
        d1.pdfs.append( Exp(amplitude=0.521208, width=0.445405))
        d1.pdfs.append(Gaus(amplitude=0.688097, mean=1., sigma=0.312677))
        d1.residuals.Set([0, 1], [1, 1])
        d1.fadc_charge_scale  = 1.
        d1.compensation_factor = 1.3

        dc1.combined_spe_charge_distribution = d1

        d2 = dataclasses.SPEChargeDistribution()
        d2.pdfs.append( Exp(amplitude=6.68282,  width=0.0342546))
        d2.pdfs.append( Exp(amplitude=0.521208, width=0.445405))
        d2.pdfs.append(Gaus(amplitude=0.688097, mean=1., sigma=0.312677))
        d2.residuals.Set([0, 1], [1, 1])
        d2.fadc_charge_scale  = 1.
        d2.compensation_factor = 1.3

        dc2.combined_spe_charge_distribution = d2

        mspe1 = dataclasses.mean_spe_charge(dc1)
        mspe2 = dataclasses.mean_spe_charge(dc2)

        self.assertEqual(mspe1, mspe2, "these should be the same.")

        max_charge = 100
        manalytic = (integrate.quad(lambda x: d1(x)*x, 0, max_charge)[0] /
                     integrate.quad(lambda x: d1(x), 0, max_charge)[0])
        manalytic *= d1.compensation_factor
        self.assertLess(manalytic-mspe1, 0.01, "these should be close.")


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

