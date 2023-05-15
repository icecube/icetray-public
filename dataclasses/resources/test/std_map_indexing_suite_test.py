#!/usr/bin/env python3
#
# std_map_indexing_suite_test.py
#
# A test suite demonstrating the additional python-side features
# of the std_map_indexing_suite wrapper for std::map
# JvS 2009-03-26

import os
import sys
import timeit
import unittest

from icecube import dataclasses, icetray

try:
	dataclasses.I3MapStringDouble.keys  # noqa: B018
	dataclasses.I3MapUnsignedUnsigned.keys  # noqa: B018
except AttributeError:
	print('You appear to be using the default map_indexing_suite. Not running tests for std_map_indexing_suite.')
	sys.exit(0)

class I3MapStringDoubleTest(unittest.TestCase):
	"""A demonstration of the more pythonic features in std_map_index_suite"""
	def setUp(self):
		self.map = dataclasses.I3MapStringDouble()
		self.sourceDict = {'foo': 1.2, 'bar': 3.7, 'baz': 42.9}
		for k in self.sourceDict:
			self.map[k] = self.sourceDict[k]
	def testAssignment(self):
		"""The values in sourceDict were copied to map"""
		for k in self.sourceDict:
			self.assertEqual(self.map[k],self.sourceDict[k])
	def testKeys(self):
		"""map.keys() returns a list of keys"""
		keys = self.map.keys()
		for k in keys:
			self.assertEqual(k in self.sourceDict, True)
	def testKeyIterables(self):
		"""Various equivalent ways of getting the keys"""
		self.assertEqual(list(self.map.keys()), [ key           for key       in self.map.keys() ])  # noqa: SIM118
		self.assertEqual(list(self.map.keys()), [ key           for key,value in self.map            ])
		self.assertEqual(list(self.map.keys()), [ entry.key()   for entry     in self.map            ])
		self.assertEqual(list(self.map.keys()), [ entry.first() for entry     in self.map            ])
	def testValueIterables(self):
		"""Various equivalent ways of getting the values"""
		self.assertEqual(list(self.map.values()), [ value          for value     in self.map.values() ])
		self.assertEqual(list(self.map.values()), [ value          for key,value in self.map              ])
		self.assertEqual(list(self.map.values()), [ entry.data()   for entry     in self.map              ])
		self.assertEqual(list(self.map.values()), [ entry.second() for entry     in self.map              ])
	def testIterators(self):
		"""Iterators work for keys, values, and (k,v) tuples"""
		self.assertEqual( type( next(iter(self.map.items()))  ),tuple)
		self.assertEqual( type( next(iter(self.map.keys()))   ), type( list(self.sourceDict.keys())[0]   ))
		self.assertEqual( type( next(iter(self.map.values())) ), type( list(self.sourceDict.values())[0] ))
	def testHasKey(self):
		"""map.has_key() is equivalent to 'key in map'"""
		key = list(self.sourceDict.keys())[0]
		self.assertEqual(key in self.map,True)
		self.assertEqual(key in self.map, key in self.map)
	def testCopyKeys(self):
		"""can produce a new, equivalent dict from sourceDict via dict.update(map)"""
		d = dict()
		d.update(self.map)
		self.assertEqual(self.sourceDict, d)
	def testPairUnpacking(self):
		"""the wrapped std::pair can be unpacked automatically"""
		for entry in self.map:
			key,value = entry
			self.assertEqual(key, entry.key())
			self.assertEqual(key, entry.first())
			self.assertEqual(value, entry.data())
			self.assertEqual(value, entry.second())
	def testPairIndexing(self):
		"""elements of the wrapped std::pair can be accessed by index"""
		for entry in self.map:
			self.assertEqual(entry[0], entry.key())
			self.assertEqual(entry[-1], entry.data())
			self.assertEqual(entry[-2], entry.key())
			self.assertEqual(entry[1], entry.data())
			self.assertRaises(IndexError, entry.__getitem__, -3)
			self.assertRaises(IndexError, entry.__getitem__, 2)
			self.assertEqual([entry.key(),entry.data()], list(entry))
			self.assertEqual(len(entry), 2)

class IterRunner:
	def setUp(self):
		self.map = dataclasses.I3MapStringDouble()
		for i in range(10000):
			v = i/1.0e4
			k = str(hash(v))
			self.map[k] = v 
	def iterClassic(self):
		return [(pair.key(),pair.data()) for pair in self.map]
	def iterClassicUnpack(self):
		return [(key,value) for key,value in self.map] 
	def getItems(self):
		return list(self.map.items())
	def getItemsUnpack(self):
		return [(key,value) for key,value in self.map.items()]
	def iterItems(self):
		return [item for item in self.map.items()]
	def iterItemsUnpack(self):
		return [(key,value) for key,value in self.map.items()]

class I3MapStringDoublePerformanceTest(unittest.TestCase):
	"""Run some quick benchmarks"""
	def setUp(self):
		self.map = dataclasses.I3MapStringDouble()
		for i in range(5000):
			v = i/5.0e3
			k = str(hash(v))
			self.map[k] = v 
	def testPerformance(self):
		this_script = os.path.basename(os.path.splitext(__file__)[0])
		setup = 'from %s import IterRunner; case = IterRunner(); case.setUp()' % this_script
		def runCase(meth,desc,num=100):
			print('%3.dx %s:' % (num,desc))
			results = timeit.Timer(stmt='case.%s()'%meth,setup=setup).timeit(num)
			print('===> %.3f s' % results)
		print('')
		print('Performance improvements (and some regressions) with the new map interfaces:')
		runCase('iterClassic','[(pair.key(),pair.data()) for pair in i3map]\t(the old way)')
		runCase('iterClassicUnpack','[(key,value) for key,value in i3map]\t\t(with implicit unpacking)')
		runCase('iterItemsUnpack','[(key,value) for key,value in i3map.iteritems()]\t(item iterator with implicit unpacking)')
		runCase('iterItems','[item for item in i3map.iteritems()]\t\t(using iterators + list comprehensions)')
		runCase('getItems','i3map.items()\t\t\t\t\t(internal iterator only)')

class I3MapUnsignedUnsignedTest(I3MapStringDoubleTest):
	def setUp(self):
		self.map = dataclasses.I3MapUnsignedUnsigned()
		self.sourceDict = {2: 1, 7: 3, 11: 42}
		for k in self.sourceDict:
			self.map[k] = self.sourceDict[k]

class I3MapKeyDoubleTest(I3MapStringDoubleTest):
	def setUp(self):
		i,d = icetray,dataclasses
		self.map = d.I3MapKeyDouble()
		self.sourceDict = {i.OMKey(0,1): 1.2, i.OMKey(7,19): 3.7, i.OMKey(11,42): 42.11}
		for k in self.sourceDict:
			self.map[k] = self.sourceDict[k]

# FIXME: the I3MapIntVectorInt wrapper will only take std::vector<int>, not I3VectorInt. why?
# class I3MapIntVectorIntTest(I3MapStringDoubleTest):
# 	def setUp(self):
# 		from icecube import icetray,dataclasses
# 		self.map = dataclasses.I3MapIntVectorInt()
# 		v1 = dataclasses.I3VectorInt() 
# 		v2 = dataclasses.I3VectorInt() 
# 		v3 = dataclasses.I3VectorInt() 
# 	  	for i in [1,2,3]: v1.append(i) 
#                 for i in [4,5,6]: v2.append(i) 
#                 for i in [7,8,9]: v3.append(i) 
# 		self.sourceDict = {1: v1, 2: v2, -42: v3}
# 		self.map[0] = v1
# 		for k in self.sourceDict.keys(): self.map[k] = self.sourceDict[k]

if __name__ == '__main__':
	unittest.main()
