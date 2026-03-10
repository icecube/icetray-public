#!/usr/bin/env python3
#
# Copyright (C) 2015 The IceCube Collaboration
# SPDX-License-Identifier: BSD-2-Clause
#
# @author Juan Carlos Diaz-Velez
# @date March 2026

"""
Test I3Astro by comparing the results to computed
ra and decs
"""

import unittest
import numpy as np
import math
import random
from icecube import icetray, dataclasses,astro
from icecube.icetray import I3Units

def fractional_day_hours(time):
    mjd = time.mod_julian_day_double
    return (mjd - int(mjd)) * 24


class GMESTTest(unittest.TestCase):



    def test_gmest_range(self):
        t = dataclasses.I3Time()
        t.set_utc_cal_date(2020,3,1,6,30,0,0)
        ast = astro.I3GetGMEST(t)

        assert 0 <= ast < 24


    def test_est_definition(self):
        t = dataclasses.I3Time()
        t.set_utc_cal_date(2020,3,1,6,30,0,0)

        gmst = astro.I3GetGMST(t)
        est  = astro.I3GetGMEST(t)

        lst = fractional_day_hours(t)
        expected = (2 * gmst - lst) % 24

        assert math.isclose(est, expected, rel_tol=1e-10)



    def test_random_times(self):
        for _ in range(100):
            M = random.randint(1,12)
            d = random.randint(1,28)
            h = random.randint(0,23)
            m = random.randint(0,59)
            s = random.randint(0,59)
            t = dataclasses.I3Time()
            t.set_utc_cal_date(2000,M,d,h,m,s,0)

            gmst = astro.I3GetGMST(t)
            est  = astro.I3GetGMEST(t)

            assert 0 <= gmst < 24
            assert 0 <= est < 24


if __name__=='__main__':
    unittest.main()


