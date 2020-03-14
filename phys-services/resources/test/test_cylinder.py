#!/usr/bin/env python
import unittest
from numpy import pi,arccos,arange
from scipy.integrate import quad
from icecube import phys_services
from icecube.dataclasses import I3Direction as i3d

class TestCylinder(unittest.TestCase):

    def setUp(self):
        self.length=100
        self.radius=200
        self.surface=phys_services.Cylinder(self.length,self.radius)

    def test_cylinder_zenith(self):
        self.assertAlmostEqual(
            self.surface.area(i3d(0,0)),
            pi*self.radius**2)
        self.assertAlmostEqual(
            self.surface.area(i3d(pi,0)),
            pi*self.radius**2)
        self.assertAlmostEqual(
            self.surface.area(i3d(pi/2,0)),
            2*self.length*self.radius)

    def test_cylinder_acceptance(self):

        integrand=lambda z:2*pi*self.surface.area(i3d(arccos(z),0))
        
        for cz1 in arange(-1,1.1,.25):
            for cz2 in arange(-1,1.1,.25):            
                I1,E1=quad(integrand,cz1,cz2)
                I2=self.surface.acceptance(cz1,cz2)
                assert(abs(I1-I2)<=E1)


if __name__ == '__main__':
    unittest.main()
    



