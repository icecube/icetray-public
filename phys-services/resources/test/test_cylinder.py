import unittest
import numpy as np
from scipy.integrate import quad
from icecube import phys_services

class TestCylinder(unittest.TestCase):

    def setUp(self):
        self.length=100
        self.radius=200
        self.surface=phys_services.Cylinder(self.length,self.radius)

    def test_cylinder_zenith(self):
        self.assertAlmostEqual(
            self.surface.GetAreaForZenith(-1),
            np.pi*self.radius**2)
        self.assertAlmostEqual(
            self.surface.GetAreaForZenith(1),
            np.pi*self.radius**2)
        self.assertAlmostEqual(
            self.surface.GetAreaForZenith(0),
            2*self.length*self.radius)                

    def test_cylinder_integral(self):        
        for cz1 in np.arange(-1,1.1,.25):
            for cz2 in np.arange(-1,1.1,.25):
                I1,E1=quad(self.surface.GetAreaForZenith,cz1,cz2)
                I2=(self.surface.GetAreaForZenithAntiDerivative(cz2)
                    -self.surface.GetAreaForZenithAntiDerivative(cz1))
                assert(abs(I1-I2)<=E1)


if __name__ == '__main__':
    unittest.main()
    



