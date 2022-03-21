#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015


"""
Test the inverse of coordinate transformations by doing forward 
transformation and then the reverse transformation and comparing 
the results
 
"""

import unittest
import random,math
from icecube.icetray import I3Units
from icecube import astro,dataclasses


class InverseTest(unittest.TestCase):

    
    def assert_almost_equal(self,a,b,tol):
        if abs(a-b)>tol:
            raise AssertionError("abs({} - {}) > {}".format(a,b,tol))

    def assert_less(self,a,b):
        if not a<b:
            raise AssertionError("{} > {}".format(a,b))

    def test_equa_from_dir(self):

        random.seed(0)
        
        t1 = dataclasses.I3Time()

        t1.set_utc_cal_date(2000,1,1,0,0,0,0.0)
        mjd1 = t1.mod_julian_day_double
        t1.set_utc_cal_date(2030,1,1,0,0,0,0.0)
        mjd2 = t1.mod_julian_day_double

        for n in range(1000):
            t1.set_mod_julian_time_double(
                random.uniform(mjd1,mjd2)
            )

            d = dataclasses.I3Direction(
                random.uniform(0,  math.pi),
                random.uniform(0,2*math.pi),
            )
        
            eq = astro.I3GetEquatorialFromDirection(d,t1)
            dprime = astro.I3GetDirectionFromEquatorial(eq,t1)

            self.assert_almost_equal(d.zenith,dprime.zenith,5e-6)
            self.assert_almost_equal(d.azimuth*math.sin(d.zenith),
                                   dprime.azimuth*math.sin(d.zenith),6e-6)

            self.assert_less(astro.angular_distance(
                d.azimuth,math.pi/2 - d.zenith,
                dprime.azimuth,math.pi/2 - dprime.zenith),6e-6)

    def test_dir_from_equa(self):

        random.seed(0)
        
        t1 = dataclasses.I3Time()

        t1.set_utc_cal_date(2000,1,1,0,0,0,0.0)
        mjd1 = t1.mod_julian_day_double
        t1.set_utc_cal_date(2030,1,1,0,0,0,0.0)
        mjd2 = t1.mod_julian_day_double
        
        for n in range(1000):
            t1.set_mod_julian_time_double(
                random.uniform(mjd1,mjd2)
            )
            
            eq = astro.I3Equatorial(random.uniform(0,2* math.pi),
                                    random.uniform(-math.pi/2,math.pi/2)
                                    )
            
            d = astro.I3GetDirectionFromEquatorial(eq,t1)
            eqprime = astro.I3GetEquatorialFromDirection(d,t1)
            
            self.assert_almost_equal(eq.ra *math.cos(eq.dec),eqprime.ra*math.cos(eqprime.dec),4e-6)
            self.assert_almost_equal(eq.dec,eqprime.dec,4e-6)

            self.assert_less(astro.angular_distance(eq.ra,eq.dec,eqprime.ra,eqprime.dec),5e-6)

    def test_gal_from_equa(self):

        random.seed(0)
                
        for n in range(1000):
            
            eq = astro.I3Equatorial(random.uniform(0,2* math.pi),
                                    random.uniform(-math.pi/2,math.pi/2),
                                    )
            
            gal = astro.I3GetGalacticFromEquatorial(eq)
            eqprime = astro.I3GetEquatorialFromGalactic(gal)

            self.assert_almost_equal(math.cos(eq.dec)*eq.ra ,math.cos(eqprime.dec)*eqprime.ra,1e-9)
            self.assert_almost_equal(eq.dec,eqprime.dec,1e-10)

            self.assert_less(astro.angular_distance(eq.ra,eq.dec,eqprime.ra,eqprime.dec),1e-10)


    def test_equa_from_gal(self):

        random.seed(0)
                
        for n in range(1000):
            
            gal = astro.I3Galactic(random.uniform(0,2*math.pi),
                                   random.uniform(-math.pi/2,math.pi/2),
                                   )

            eq = astro.I3GetEquatorialFromGalactic(gal)
            galprime = astro.I3GetGalacticFromEquatorial(eq)

            
            self.assert_almost_equal(math.cos(gal.b)*gal.l,math.cos(galprime.b)*galprime.l,1e-9)
            self.assert_almost_equal(gal.b,galprime.b,1e-10)

            self.assert_less(astro.angular_distance(gal.l,gal.b,galprime.l,galprime.b),1e-10)

    
    def test_sg_from_equa(self):

        random.seed(0)
                
        for n in range(1000):
            
            eq = astro.I3Equatorial(random.uniform(0,2* math.pi),
                                    random.uniform(-math.pi/2,math.pi/2),
                                    )
            
            sg = astro.I3GetSuperGalacticFromEquatorial(eq)
            eqprime = astro.I3GetEquatorialFromSuperGalactic(sg)

            self.assert_almost_equal(math.cos(eq.dec)*eq.ra ,math.cos(eqprime.dec)*eqprime.ra,1e-9)
            self.assert_almost_equal(eq.dec,eqprime.dec,1e-10)
            self.assert_less(astro.angular_distance(eq.ra,eq.dec,eqprime.ra,eqprime.dec),1e-10)

    def test_equa_from_sg(self):

        random.seed(0)
                
        for n in range(1000):
            
            sg = astro.I3SuperGalactic(random.uniform(0,2*math.pi),
                                       random.uniform(-math.pi/2,math.pi/2),
            )

            eq = astro.I3GetEquatorialFromSuperGalactic(sg)
            sgprime = astro.I3GetSuperGalacticFromEquatorial(eq)
            
            self.assert_almost_equal(math.cos(sg.b)*sg.l,math.cos(sgprime.b)*sgprime.l,1e-9)
            self.assert_almost_equal(sg.b,sgprime.b,1e-10)
            self.assert_less(astro.angular_distance(sg.l,sg.b,sgprime.l,sgprime.b),1e-10)
            
    def test_sg_from_gal(self):

        random.seed(0)
        for n in range(1000):
            
            gal = astro.I3Galactic(random.uniform(0,2*math.pi),
                                   random.uniform(-math.pi/2,math.pi/2),
                                   )

            sg = astro.I3GetSuperGalacticFromGalactic(gal)
            galprime = astro.I3GetGalacticFromSuperGalactic(sg)

            
            self.assert_almost_equal(math.cos(gal.b)*gal.l,math.cos(galprime.b)*galprime.l,1e-9)
            self.assert_almost_equal(gal.b,galprime.b,1e-10)
            self.assert_less(astro.angular_distance(gal.l,gal.b,galprime.l,galprime.b),1e-10)


    def test_gal_from_sg(self):

        random.seed(0)
        for n in range(1000):
            
            sg = astro.I3SuperGalactic(random.uniform(0,2*math.pi),
                                        random.uniform(-math.pi/2,math.pi/2),
            )

            gal = astro.I3GetGalacticFromSuperGalactic(sg)
            sgprime = astro.I3GetSuperGalacticFromGalactic(gal)

            
            self.assert_almost_equal(math.cos(sg.b)*sg.l,math.cos(sgprime.b)*sgprime.l,1e-9)
            self.assert_almost_equal(sg.b,sgprime.b,1e-10)
            self.assert_less(astro.angular_distance(sg.l,sg.b,sgprime.l,sgprime.b),1e-10)


    
            
if __name__=='__main__':
    unittest.main()
