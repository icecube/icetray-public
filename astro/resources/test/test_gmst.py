#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
Test I3Astro by comparing the results to computed 
ra and decs 
"""

import unittest
import numpy as np
from icecube import icetray, dataclasses,astro
from icecube.icetray import I3Units

def azimuth_distance(a,b):
    d = abs(a-b)
    return min(d,360-d)

class GMSTTest(unittest.TestCase):

    def caclulate_gmst(self,mjd):
        #magic numbers
        j2k = 51544.5 # J2000
        dd = 36525. 
        c0 = 280.46061837
        c1 = 1.31850007701e+07
        c2 = 3.87933e-04
        c3 = -2.58331181e-08
        T = (mjd-j2k)/dd
        tsid = c0+(c1+(c2+c3*T)*T)*T
        tsid = tsid%360
        if tsid < 0: tsid += 360
        return tsid*24/360

    def test_gmst(self):

        i3t0 = dataclasses.I3Time()
        i3t0.set_utc_cal_date(2000,1,1,0,0,0,0)

        for day in np.linspace(0,365*100,1): # run over a century
            for hour in np.linspace(0,24,1):
                for minute in np.linspace(0,60,1):
                    i3time = i3t0+day*minute*60*I3Units.s
                
                    i3gmst_a = astro.I3GetGMST(i3time)
                    i3gmst_b = self.caclulate_gmst(i3time.mod_julian_day_double)
                    assert (abs(i3gmst_a-i3gmst_b) <0.000002)


if __name__=='__main__':
    unittest.main()
        

