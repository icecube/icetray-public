#!/usr/bin/env python

import unittest
import sys

from icecube import icetray,dataclasses

class I3RecoPulseSeriesMapMaskTest(unittest.TestCase):
	def setUp(self):
		self.frame = icetray.I3Frame(icetray.I3Frame.Physics)
		pulses = dataclasses.I3RecoPulseSeriesMap()
		key1 = icetray.OMKey(42, 7)
		vec = dataclasses.I3RecoPulseSeries()
		pulse = dataclasses.I3RecoPulse()
		pulse.time = 1.0
		pulse.charge = 2.3
		vec.append(pulse)
		pulse.time = 2.0
		vec.append(pulse)
		pulse.time = 15.0
		vec.append(pulse)
		pulses[key1] = vec
		
		key2 = icetray.OMKey(7,7)
		vec = dataclasses.I3RecoPulseSeries()
		pulse.time = 1.0
		pulse.charge = 2.3
		vec.append(pulse)
		pulse.time = 2.0
		vec.append(pulse)
		pulse.time = 15.0
		vec.append(pulse)
		pulses[key2] = vec
		
		self.frame['Pulses'] = pulses
	
		mask1 = dataclasses.I3RecoPulseSeriesMapMask(self.frame, 'Pulses')
		mask1.set(key1, 1, False)
		self.frame['Mask1'] = mask1
		
		mask2 = dataclasses.I3RecoPulseSeriesMapMask(self.frame, 'Pulses')
		mask2.set(key2, 1, False)
		self.frame['Mask2'] = mask2
	def testApply(self):
		mask = self.frame['Mask1']
		pulses = mask.apply(self.frame)
		self.assertEquals(len(pulses), 2)
		self.assertEquals(len(pulses.values()[0]), 3)
		self.assertEquals(len(pulses.values()[1]), 2)
		
		mask = self.frame['Mask2']
		pulses = mask.apply(self.frame)
		self.assertEquals(len(pulses), 2)
		self.assertEquals(len(pulses.values()[0]), 2)
		self.assertEquals(len(pulses.values()[1]), 3)
		
	def testCombine(self):
		mask1 = self.frame['Mask1']
		mask2 = self.frame['Mask2']
		
		combined = mask1 & mask2
		pulses = combined.apply(self.frame)
		self.assertEquals(len(pulses), 2)
		self.assertEquals(len(pulses.values()[0]), 2)
		self.assertEquals(len(pulses.values()[1]), 2)
		
		combined = mask1 | mask2
		pulses = combined.apply(self.frame)
		self.assertEquals(len(pulses), 2)
		self.assertEquals(len(pulses.values()[0]), 3)
		self.assertEquals(len(pulses.values()[1]), 3)
		
	def testQuery(self):
		mask1 = self.frame['Mask1']
		
		self.assertEquals(mask1.any(), True)
		self.assertEquals(mask1.all(), False)
		self.assertEquals(mask1.sum(), 5)
		
		mask1.set_none()
		
		self.assertEquals(mask1.any(), False)
		self.assertEquals(mask1.sum(), 0)
	
	def testEqual(self):
		mask1 = self.frame['Mask1']
		mask2 = self.frame['Mask2']
		mask3 = dataclasses.I3RecoPulseSeriesMapMask(mask1)
		
		self.assertNotEquals(mask1,mask2)
		
		self.assertEquals(mask1.source,mask3.source)
		self.assertEquals(mask1.bits,mask3.bits)
		self.assertEquals(mask1,mask3)
		self.assertEquals(mask1 != mask3,False)
		
if __name__ == '__main__':
	unittest.main()
