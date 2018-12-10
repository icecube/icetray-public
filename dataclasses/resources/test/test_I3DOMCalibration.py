#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3DOMCalibration(unittest.TestCase):

    def test_LinearFit_equality(self):
        fit1 = dataclasses.LinearFit()
        fit1.slope = 1.618
        fit1.intercept = 1.20205

        fit2 = dataclasses.LinearFit()
        fit2.slope = 1.618
        fit2.intercept = 1.20205
        
        self.assertEqual(fit1, fit2, "these should be the same.")

        fit2.slope = 0.
        self.assertNotEqual(fit1, fit2, "these should be the same.")

    def test_QuadraticFit_equality(self):
        fit1 = dataclasses.QuadraticFit()
        fit1.quad_fit_a = 1.618
        fit1.quad_fit_b = 1.645
        fit1.quad_fit_c = 1.20205

        fit2 = dataclasses.QuadraticFit()
        fit2.quad_fit_a = 1.618
        fit2.quad_fit_b = 1.645
        fit2.quad_fit_c = 1.20205
        
        self.assertEqual(fit1, fit2, "these should be the same.")

        fit2.quad_fit_a = 0.
        self.assertNotEqual(fit1, fit2, "these should be the same.")

    def test_SPEChargeDistribution_equality(self):
        d1 = dataclasses.SPEChargeDistribution()
        d1.exp1_amp   = 6.68282
        d1.exp1_width = 0.0342546
        d1.exp2_amp   = 0.521208
        d1.exp2_width = 0.445405
        d1.gaus_amp   = 0.688097
        d1.gaus_mean  = 1.
        d1.gaus_width = 0.312677
        d1.compensation_factor = 1.3

        d2 = dataclasses.SPEChargeDistribution()
        d2.exp1_amp   = 6.68282
        d2.exp1_width = 0.0342546
        d2.exp2_amp   = 0.521208
        d2.exp2_width = 0.445405
        d2.gaus_amp   = 0.688097
        d2.gaus_mean  = 1.
        d2.gaus_width = 0.312677
        d2.compensation_factor = 1.3        

        self.assertEqual(d1, d2, "these should be the same.")

        d2.exp1_amp = 0.
        self.assertNotEqual(d1, d2, "these should be the same.")

    def test_TauParam_equality(self):
        d1 = dataclasses.TauParam()
        d1.p0 = 0.5772
        d1.p1 = 1.645
        d1.p2 = 1.202
        d1.p3 = 1.0823
        d1.p4 = 2.612
        d1.p5 = -1.4604
        d1.tau_frac = -0.5
        
        d2 = dataclasses.TauParam()
        d2.p0 = 0.5772
        d2.p1 = 1.645
        d2.p2 = 1.202
        d2.p3 = 1.0823
        d2.p4 = 2.612
        d2.p5 = -1.4604
        d2.tau_frac = -0.5
       
        self.assertEqual(d1, d2, "these should be the same.")

        d2.p0 = 0.
        self.assertNotEqual(d1, d2, "these should be the same.")

    def test_DroopedSPETemplate_equality(self):
        temp1 = dataclasses.SPETemplate()
        temp1.c = 1.89
        temp1.x0= 1.2
        temp1.b1= 1.2
        temp1.b2= 1.1

        temp2 = dataclasses.SPETemplate()
        temp2.c = 1.89
        temp2.x0= 1.2
        temp2.b1= 1.2
        temp2.b2= 1.1

        droop1 = dataclasses.DroopedSPETemplate(temp1,temp2,1.1,1.1,1.1)
        droop2 = dataclasses.DroopedSPETemplate(temp1,temp2,1.1,1.1,1.1)
         
        self.assertEqual(droop1, droop2, "these should be the same.")

        droop3 = dataclasses.DroopedSPETemplate(temp1,temp2,1.1,1.1,0.)
        self.assertNotEqual(droop1, droop3, "these should be the same.")


    def test_I3DOMCalibration_equality_default_ctor(self):
        dc1 = dataclasses.I3DOMCalibration()
        dc2 = dataclasses.I3DOMCalibration()
        self.assertEqual(dc1, dc2, "these should be the same.")

    def test_SPETemplate_equality_default_ctor(self):
        o1 = dataclasses.SPETemplate()
        o2 = dataclasses.SPETemplate()
        self.assertEqual(o1, o2, "these should be the same.")

    def test_LinearFit_equality_default_ctor(self):
        o1 = dataclasses.LinearFit()
        o2 = dataclasses.LinearFit()
        self.assertEqual(o1, o2, "these should be the same.")

    def test_QuadraticFit_equality_default_ctor(self):
        o1 = dataclasses.QuadraticFit()
        o2 = dataclasses.QuadraticFit()
        self.assertEqual(o1, o2, "these should be the same.")

    def test_TauParam_equality_default_ctor(self):
        o1 = dataclasses.TauParam()
        o2 = dataclasses.TauParam()
        self.assertEqual(o1, o2, "these should be the same.")

    def test_SPEChargeDistribution_equality_default_ctor(self):
        o1 = dataclasses.SPEChargeDistribution()
        o2 = dataclasses.SPEChargeDistribution()
        self.assertEqual(o1, o2, "these should be the same.")


    def test_LinearFit_string(self):
        fit1 = dataclasses.LinearFit()
        fit1.slope = 1.618
        fit1.intercept = 1.20205
        fit2 = dataclasses.LinearFit()
        fit2.slope = 1.618
        fit2.intercept = 1.20205

        self.assertEqual(fit1.__str__(), fit2.__str__(), "these should be the same.")


    def test_QuadraticFit_string(self):
        fit1 = dataclasses.QuadraticFit()
        fit1.quad_fit_a = 1.618
        fit1.quad_fit_b = 1.645
        fit1.quad_fit_c = 1.20205
        fit2 = dataclasses.QuadraticFit()
        fit2.quad_fit_a = 1.618
        fit2.quad_fit_b = 1.645
        fit2.quad_fit_c = 1.20205

        self.assertEqual(fit1.__str__(), fit2.__str__(), "these should be the same.")


    def test_SPEChargeDistribution_string(self):
        d1 = dataclasses.SPEChargeDistribution()
        d1.exp1_amp   = 6.68282
        d1.exp1_width = 0.0342546
        d1.exp2_amp   = 0.521208
        d1.exp2_width = 0.445405
        d1.gaus_amp   = 0.688097
        d1.gaus_mean  = 1.
        d1.gaus_width = 0.312677
        d1.compensation_factor = 1.3

        d2 = dataclasses.SPEChargeDistribution()
        d2.exp1_amp   = 6.68282
        d2.exp1_width = 0.0342546
        d2.exp2_amp   = 0.521208
        d2.exp2_width = 0.445405
        d2.gaus_amp   = 0.688097
        d2.gaus_mean  = 1.
        d2.gaus_width = 0.312677
        d2.compensation_factor = 1.3
        
        self.assertEqual(d1.__str__(), d2.__str__(), "these should be the same.")


    def test_TauParam_string(self):
        d1 = dataclasses.TauParam()
        d1.p0 = 0.5772
        d1.p1 = 1.645
        d1.p2 = 1.202
        d1.p3 = 1.0823
        d1.p4 = 2.612
        d1.p5 = -1.4604
        d1.tau_frac = -0.5
        d2 = dataclasses.TauParam()
        d2.p0 = 0.5772
        d2.p1 = 1.645
        d2.p2 = 1.202
        d2.p3 = 1.0823
        d2.p4 = 2.612
        d2.p5 = -1.4604
        d2.tau_frac = -0.5

        self.assertEqual(d1.__str__(), d2.__str__(), "these should be the same.")


    def test_I3DOMCalibration_string(self):
        dc1 = dataclasses.I3DOMCalibration()
        dc2 = dataclasses.I3DOMCalibration()

        self.assertEqual(dc1.__str__(), dc2.__str__(), "these should be the same.")


unittest.main()
