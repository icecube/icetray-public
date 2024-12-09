#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

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
    k = dataclasses.I3MapStringDouble.keys
    k = dataclasses.I3MapUnsignedUnsigned.keys
    del k
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
        self.assertEqual(list(self.map.keys()), [ key           for key,value in self.map.items()    ])
    def testValueIterables(self):
        """Various equivalent ways of getting the values"""
        self.assertEqual(list(self.map.values()), [ value          for value     in self.map.values() ])
        self.assertEqual(list(self.map.values()), [ value          for key,value in self.map.items()  ])
    def testIterators(self):
        """Iterators work for keys, values, and (k,v) tuples"""
        self.assertEqual( type( next(iter(self.map.items()))  ),tuple)
        self.assertEqual( type( next(iter(self.map.keys()))   ), type( list(self.sourceDict.keys())[0]   ))
        self.assertEqual( type( next(iter(self.map.values())) ), type( list(self.sourceDict.values())[0] ))
    def testContains(self):
        """key in map"""
        key = list(self.sourceDict.keys())[0]
        self.assertEqual(key in self.map,True)
    def testCopyKeys(self):
        """can produce a new, equivalent dict from sourceDict via dict.update(map)"""
        d = dict()
        d.update(self.map)
        self.assertEqual(self.sourceDict, d)

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
#     def setUp(self):
#         from icecube import icetray,dataclasses
#         self.map = dataclasses.I3MapIntVectorInt()
#         v1 = dataclasses.I3VectorInt()
#         v2 = dataclasses.I3VectorInt()
#         v3 = dataclasses.I3VectorInt()
#           for i in [1,2,3]: v1.append(i)
#                 for i in [4,5,6]: v2.append(i)
#                 for i in [7,8,9]: v3.append(i)
#         self.sourceDict = {1: v1, 2: v2, -42: v3}
#         self.map[0] = v1
#         for k in self.sourceDict.keys(): self.map[k] = self.sourceDict[k]

if __name__ == '__main__':
    unittest.main()
