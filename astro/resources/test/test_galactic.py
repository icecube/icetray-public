#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
Test astro's Galactic coordinate transformations by checking
against a static list. The list came from as SIMBAD query

http://simbad.u-strasbg.fr/simbad/sim-fid
 
"""

import sys,os
if sys.version_info < (2,7):
    import unittest2 as unittest
else:
    import unittest

from icecube.icetray import I3Units
from icecube import astro


TEST_DATA = os.path.expandvars("$I3_BUILD/astro/resources/test/")

class GalacticTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):

        cls.tevcat = []
        with open(TEST_DATA + "simbad_tevcat_galactic.txt") as f:
            for line in f.readlines()[7:]:
                line = line.split('|')
                if len(line)!=7:
                    continue
                cls.tevcat.append([ [float(b) for b in a.split()] for a in line[3:] ])
            
    def test_reference_points(self):
        #Test the reference points that define the galactic coordinate system
        #take from Wikipedia, the orignal paper was in B1950

        #celestial north pole        
        eq = astro.I3Equatorial((12+51.4/60.)*15*I3Units.degree,
                                27.13 * I3Units.degree)
        gal = astro.I3GetGalacticFromEquatorial(eq)
        self.assertAlmostEqual(gal.b/I3Units.degree,+90,1)

        #south pole
        eq.ra  = (+51.4/60.)*15*I3Units.degree
        eq.dec = -27.13 * I3Units.degree
        gal = astro.I3GetGalacticFromEquatorial(eq)
        self.assertAlmostEqual(gal.b/I3Units.degree,-90,1)

        #galactic center 
        eq.ra  = (17+45.6/60.)*15*I3Units.degree
        eq.dec = -28.94 * I3Units.degree
        gal = astro.I3GetGalacticFromEquatorial(eq)
        self.assertAlmostEqual(gal.b/I3Units.degree,0,1)
        self.assertAlmostEqual(gal.l/I3Units.degree,360,1)

        #anti-center 
        eq.ra  = (5+45.6/60.)*15*I3Units.degree
        eq.dec = +28.94 * I3Units.degree
        gal = astro.I3GetGalacticFromEquatorial(eq)
        self.assertAlmostEqual(gal.b/I3Units.degree,0,1)
        self.assertAlmostEqual(gal.l/I3Units.degree,180,1)

        #now test the reverse transform

        #galactic north pole
        gal = astro.I3Galactic(  0 * I3Units.degree,
                               +90 * I3Units.degree)

        eq = astro.I3GetEquatorialFromGalactic(gal)
        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               (12+51.4/60.)*15.,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               27.13,1)
        

        #south pole
        gal.l  =   0 * I3Units.degree
        gal.b  = -90 * I3Units.degree
        eq = astro.I3GetEquatorialFromGalactic(gal)
        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               (0+51.4/60.)*15.,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               -27.13,1)

        #galactic center
        gal.l  =   0 * I3Units.degree
        gal.b  =   0 * I3Units.degree
        eq = astro.I3GetEquatorialFromGalactic(gal)
        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               (17+45.6/60.)*15.,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               -28.94,1)

        #anti center
        gal.l  =   0 * I3Units.degree
        gal.b  = 180 * I3Units.degree
        eq = astro.I3GetEquatorialFromGalactic(gal)
        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               (5+45.6/60.)*15.,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               +28.94,1)


        #supergalactic north pole
        sg = astro.I3SuperGalactic()
        sg.b = 90 *I3Units.degree
        sg.l =  0 *I3Units.degree
        gal = astro.I3GetGalacticFromSuperGalactic(sg)

        self.assertAlmostEqual(gal.l/I3Units.degree,
                               47.37,6)
        self.assertAlmostEqual(gal.b/I3Units.degree,
                               6.32,6)

        eq = astro.I3GetEquatorialFromSuperGalactic(sg)

        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               18.92*15,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               15.7,1)
        

        #supergalactic origin
        sg = astro.I3SuperGalactic()
        sg.b =  0 *I3Units.degree
        sg.l =  0 *I3Units.degree
        gal = astro.I3GetGalacticFromSuperGalactic(sg)

        self.assertAlmostEqual(gal.l/I3Units.degree,
                               137.37,6)
        self.assertAlmostEqual(gal.b/I3Units.degree,
                               0,6)

        eq = astro.I3GetEquatorialFromSuperGalactic(sg)

        self.assertAlmostEqual(eq.ra/I3Units.degree,
                               2.82*15,1)
        self.assertAlmostEqual(eq.dec/I3Units.degree,
                               59.5,1)


        
    def test_equa_to_gal(self):
        for line in self.tevcat:
            ra,dec  = line[1]
            l,b     = line[3]
            
            eq = astro.I3Equatorial( ra*I3Units.degree,
                                    dec*I3Units.degree)
            
            gal = astro.I3GetGalacticFromEquatorial(eq)
            self.assertAlmostEqual(gal.l/I3Units.degree,l,3)
            self.assertAlmostEqual(gal.b/I3Units.degree,b,3)

    def test_gal_to_equa(self):
        for line in self.tevcat:
            ra,dec  = line[1]
            l,b     = line[3]

            gal = astro.I3Galactic(l*I3Units.degree,
                                   b*I3Units.degree)
            eq = astro.I3GetEquatorialFromGalactic(gal)
            
            self.assertAlmostEqual(eq.ra/I3Units.degree,ra,3)
            self.assertAlmostEqual(eq.dec/I3Units.degree,dec,3)

    def test_equa_to_sg(self):
        for line in self.tevcat:
            ra,dec  = line[1]
            sgl,sgb = line[2]            
            eq = astro.I3Equatorial( ra*I3Units.degree,
                                    dec*I3Units.degree)            
            sg = astro.I3GetSuperGalacticFromEquatorial(eq)
            
            self.assertAlmostEqual(sg.l/I3Units.degree,sgl,2)
            self.assertAlmostEqual(sg.b/I3Units.degree,sgb,2)


    def test_sg_to_equa(self):

        for line in self.tevcat:
            ra,dec  = line[1]
            sgl,sgb = line[2]
            sg = astro.I3SuperGalactic(sgl*I3Units.degree,
                                       sgb*I3Units.degree)
            eq = astro.I3GetEquatorialFromSuperGalactic(sg)
            
            self.assertAlmostEqual(eq.ra/I3Units.degree,ra,2)
            self.assertAlmostEqual(eq.dec/I3Units.degree,dec,2)

    def test_gal_to_sg(self):
        for line in self.tevcat:
            l,b     = line[3]
            sgl,sgb = line[2]            
            gal = astro.I3Galactic(l*I3Units.degree,
                                   b*I3Units.degree)            
            sg = astro.I3GetSuperGalacticFromGalactic(gal)
            
            self.assertAlmostEqual(sg.l/I3Units.degree,sgl,2)
            self.assertAlmostEqual(sg.b/I3Units.degree,sgb,2)


    def test_sg_to_gal(self):
        for line in self.tevcat:
            l,b     = line[3]
            sgl,sgb = line[2]
            sg = astro.I3SuperGalactic(sgl*I3Units.degree,
                                       sgb*I3Units.degree)
            gal = astro.I3GetGalacticFromSuperGalactic(sg)
            
            self.assertAlmostEqual(gal.l/I3Units.degree,l,2)
            self.assertAlmostEqual(gal.b/I3Units.degree,b,2)


if __name__=='__main__':
    unittest.main()
