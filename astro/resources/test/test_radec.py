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

class AstroTest(unittest.TestCase):

    def test_radec(self):

        src_dec = 22.01450
        for src_ra  in np.linspace(0,360,1):

            i3src = astro.I3Equatorial(src_ra*I3Units.degree,
                                       src_dec*I3Units.degree)
            
            i3t0 = dataclasses.I3Time()
            i3t0.set_utc_cal_date(2000,1,1,0,0,0,0)
            
            for day in np.linspace(0,10000,100):
                i3time = i3t0+day*86400*I3Units.s
                
                i3dir = astro.I3GetDirectionFromEquatorial(i3src,i3time)
                i3az =   i3dir.azimuth/I3Units.degree
                i3zen =  i3dir.zenith/I3Units.degree
                
                #this equation from inspecting the output of ephempy
                paz = ( 189.9640733918 -src_ra  + day*360.985606808)%360
                pzen = 90 + src_dec
                
                assert (azimuth_distance(i3az,paz) <0.2)
                assert( abs(i3zen - pzen) <0.2)


    def test_nan(self):
        eq = astro.I3Equatorial()

        assert(np.isnan(eq.ra))
        assert(np.isnan(eq.dec))

        gal = astro.I3Galactic()
        assert(np.isnan(gal.l))
        assert(np.isnan(gal.b))

        

if __name__=='__main__':
    unittest.main()
        

