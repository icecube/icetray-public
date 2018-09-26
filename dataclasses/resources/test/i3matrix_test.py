#!/usr/bin/env python

import sys, os
import unittest

from icecube import icetray, dataclasses, dataio
from icecube.dataclasses import I3Matrix

try:
	import numpy
except ImportError:
	print('This test requires numpy to function')
	sys.exit(0)

class I3MatrixTest(unittest.TestCase):
	
	def testAsArray(self):
		"""
		I3Matrix can be viewed through numpy.asarray()
		"""
		m = I3Matrix(3,2)
		a = numpy.asarray(m)
		self.assertEquals(a.shape, (3,2))
		self.assertEquals(a.dtype, numpy.float64)
	
	def testRefCounting(self):
		"""
		The numpy.asarray() view keeps the I3Matrix alive
		"""
		def factory():
			# NB: m leaves scope when the function returns
			m = I3Matrix(3,2)
			a = numpy.asarray(m)
			return (a, id(m))
		a, mid = factory()
		self.assertEquals(id(a.base), mid)
	
	def testArrayInitialization(self):
		"""
		I3Matrix can be initialized with an arbitrary value
		"""
		m = I3Matrix(3,2,numpy.pi)
		a = numpy.asarray(m)
		self.assert_((a == numpy.pi).all())
	
	def testFromNumpy(self):
		self.assertRaises(TypeError, I3Matrix, 1)
		
		# 1-D arrays fail
		self.assertRaises(ValueError, I3Matrix, numpy.array([0.,1.]))
		# > 2-D arrays fail
		self.assertRaises(ValueError, I3Matrix, numpy.ones((3,3,3)))
		# integer arrays fail
		self.assertRaises(TypeError, I3Matrix, numpy.eye(3, dtype=numpy.int32))
		# as do single-precision arrays
		self.assertRaises(TypeError, I3Matrix, numpy.eye(3, dtype=numpy.float32))
		# and compound types
		self.assertRaises(TypeError, I3Matrix, numpy.zeros((1,1), dtype=numpy.dtype([('foo', numpy.float64), ('bar', numpy.float64)])))
		
		# strided arrays are not handled
		strider = numpy.eye(3)[::2]
		self.assertNotEqual(strider.__array_interface__['strides'], None)
		self.assertRaises(ValueError, I3Matrix, strider)
		
		# I3Matrix is a copy, not a view
		orig = numpy.zeros((3,3))
		mat = I3Matrix(orig)
		view = numpy.asarray(mat)
		
		self.assert_((view == orig).all())
		view += 1
		self.assert_((orig == 0).all())
		self.assert_((view == 1).all())
	
	def testRoundTrip(self):
		"""
		I3Matrix serializes and deserializes properly.
		"""
		fname = 'i3matrix_test.i3'
		orig = numpy.pi*numpy.ones((3,3))
		frame = icetray.I3Frame()
		frame['foo'] = I3Matrix(orig)
		f = dataio.I3File(fname, 'w')
		f.push(frame)
		f.close()
		
		frame = dataio.I3File(fname).pop_frame()
		view = numpy.asarray(frame['foo'])
		self.assertEquals(view.shape, orig.shape)
		self.assert_((view == orig).all())
		
		os.unlink(fname)
		

if __name__ == "__main__":
	unittest.main()
