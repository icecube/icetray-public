#!/usr/bin/env python3

import unittest
import math
import numpy as np

from icecube.dataclasses import I3ComplexVector, I3Position, I3Direction

class TestI3ComplexVector(unittest.TestCase):

    x1 = 1+3j
    y1 = 5-6j
    z1 = -2j

    x2 = 8-8j
    y2 = 5
    z2 = 1-1j

    def AlmostEqComplex(self, x, y, tol=10):
        self.assertAlmostEqual(abs(x - y), 0, tol)

    def test_Constructors(self):
        vec = I3ComplexVector()
        self.assertTrue(math.isnan(vec.x.real))
        self.assertTrue(0 ==vec.x.imag)
        self.assertTrue(math.isnan(vec.y.real))
        self.assertTrue(0 ==vec.y.imag)
        self.assertTrue(math.isnan(vec.z.real))
        self.assertTrue(0 ==vec.z.imag)

        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        self.assertTrue(vec.x == self.x1)
        self.assertTrue(vec.y == self.y1)
        self.assertTrue(vec.z == self.z1)

        pos = I3Position(1,2,3)
        vec = I3ComplexVector(pos)
        for i in range(3):
            self.assertEqual(pos[i], vec[i])

        direc = I3Direction(.1,.2,.3)
        vec = I3ComplexVector(direc)
        self.assertEqual(direc.x, vec.x)
        self.assertEqual(direc.y, vec.y)
        self.assertEqual(direc.z, vec.z)

    def test_indexing(self):
        vec = I3ComplexVector()
        vec[0] = self.x1
        vec[1] = self.y1
        vec[2] = self.z1

        self.assertTrue(vec.x == self.x1)
        self.assertTrue(vec.y == self.y1)
        self.assertTrue(vec.z == self.z1)
        self.assertTrue(vec[0] == self.x1)
        self.assertTrue(vec[1] == self.y1)
        self.assertTrue(vec[2] == self.z1)

        self.assertEqual(len(vec), 3)

    def test_magnitude(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        expectedMag = np.sqrt(abs(self.x1)**2 + abs(self.y1)**2 + abs(self.z1)**2)
        self.assertAlmostEqual(vec.magnitude(), expectedMag, 10)
        self.assertAlmostEqual(abs(vec), expectedMag, 10)

        vec.normalize()
        self.assertAlmostEqual(abs(vec), 1., 10)

    def test_rotations(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)

        vec.rotate_x(np.pi/2.)
        self.assertAlmostEqual(abs(vec.x - self.x1), 0, 10)
        self.assertAlmostEqual(abs(vec.y - -1*self.z1), 0, 10)
        self.assertAlmostEqual(abs(vec.z - self.y1), 0, 10)

        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        vec.rotate_y(np.pi/2.)
        self.assertAlmostEqual(abs(vec.x - self.z1), 0, 10)
        self.assertAlmostEqual(abs(vec.y - self.y1), 0, 10)
        self.assertAlmostEqual(abs(vec.z - -1*self.x1), 0, 10)

        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        vec.rotate_z(np.pi/2.)
        self.assertAlmostEqual(abs(vec.x - -1*self.y1), 0, 10)
        self.assertAlmostEqual(abs(vec.y - self.x1), 0, 10)
        self.assertAlmostEqual(abs(vec.z - self.z1), 0, 10)

    def test_operators_self(self):
        vec1 = I3ComplexVector(self.x1, self.y1, self.z1)
        vec2 = I3ComplexVector(self.x2, self.y2, self.z2)

        #Addition
        self.AlmostEqComplex(vec1+vec2, I3ComplexVector(self.x1 + self.x2, self.y1 + self.y2, self.z1 + self.z2))
        vec1 += vec2
        self.assertTrue(vec1 != vec2)
        self.AlmostEqComplex(vec1, I3ComplexVector(self.x1 + self.x2, self.y1 + self.y2, self.z1 + self.z2))

        #Subtraction
        vec1 = I3ComplexVector(self.x1, self.y1, self.z1)
        vec2 = I3ComplexVector(self.x2, self.y2, self.z2)

        self.AlmostEqComplex(vec1-vec2, I3ComplexVector(self.x1 - self.x2, self.y1 - self.y2, self.z1 - self.z2))
        vec1 -= vec2
        self.assertTrue(vec1 != vec2)
        self.AlmostEqComplex(vec1, I3ComplexVector(self.x1 - self.x2, self.y1 - self.y2, self.z1 - self.z2))

        #Dot product
        vec1 = I3ComplexVector(self.x1, self.y1, self.z1)
        vec2 = I3ComplexVector(self.x2, self.y2, self.z2)
        self.AlmostEqComplex(vec1*vec1, abs(vec1)**2)
        self.AlmostEqComplex(vec2*vec2, abs(vec2)**2)
        self.AlmostEqComplex(vec1*vec1, vec1.Dot(vec1))

    def test_operators_scalar(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        value = 9.838105

        #Multiplication
        self.AlmostEqComplex(vec*value, I3ComplexVector(self.x1 * value, self.y1  * value, self.z1  * value))
        vec *= value
        self.AlmostEqComplex(vec, I3ComplexVector(self.x1 * value, self.y1  * value, self.z1  * value))

        #Division
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        self.AlmostEqComplex(vec/value, I3ComplexVector(self.x1/value, self.y1/value, self.z1/value))
        vec /= value
        self.AlmostEqComplex(vec, I3ComplexVector(self.x1/value, self.y1/value, self.z1/value))

    def test_string(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        thing = str(vec)

    def test_magnitude(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        self.assertTrue(abs(vec) == vec.magnitude())
        self.AlmostEqComplex(vec/abs(vec), vec.unit_vector())

    def test_print(self):
        vec = I3ComplexVector(self.x1, self.y1, self.z1)
        print(vec)

unittest.main()
