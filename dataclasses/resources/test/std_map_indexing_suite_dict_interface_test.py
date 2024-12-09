#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# std_map_indexing_suite_dict_interface_test.py
#
# A test demonstrating the dict methods that work
# with std_map_indexing_suite.
# JvS 2009-03-27

import unittest, sys
from typing import Iterator

try:
    sorted([3,2,1])
except NameError:
    print('sorted() is not defined, providing a work-around for python < 2.3')
    import copy
    def sorted(lst):
        cpy = list(copy.copy(lst))
        cpy.sort()
        return cpy

from icecube import icetray,dataclasses

class I3MapDictInterfaceTest(unittest.TestCase):
    """Kick the tires on the dict interface.

    This case has tests defined for all methods of
    dict in Python 2.5."""
    # Non-implemented methods:
    # comparison (all)
    # __hash__
    # __reduce__, __reduce_ex__ (for pickling support)
    # setdefault (would have to determine a default for every C++ type)
    # fromkeys(k,None) (ditto)
    # update (kwargs as keys can only work with string maps)
    def setUp(self):
        self.mapClass = icetray.map_string_pyobject
        self.dict = {'foo':'bar', 'bom':'baz', 'steam':'locomotive'}
        self.map = self.mapClass()
        for k in self.dict.keys(): self.map[k] = self.dict[k]

    def test___class__(self):
        """Obviously not the same class. Skip."""
        pass
    def test___cmp__(self):
        """Not implementing comparison for now. Skip."""
        pass
    def test___contains__(self):
        """dict.__contains__() is equivalent to map.__contains__()"""
        for k in self.dict.keys():
            self.assertEqual(k in self.dict, k in self.map)
        nokey = 'not a key'
        self.assertEqual(nokey in self.dict, nokey in self.map)
        pass
    def test___delattr__(self):
        """Inherited from <object>. Skip."""
        pass
    def test___delitem__(self):
        """dict.__delitem__() is equivalent to map.__delitem__()"""
        k = list(self.dict.keys())[0]
        self.assertEqual(self.dict.__delitem__(k), self.map.__delitem__(k))
        self.assertEqual(k in self.map, k in self.dict)
        pass
    def test___doc__(self):
        """Irrelevant. Skip."""
        pass
    def test___eq__(self):
        """Not implementing comparison for now. Skip."""
        pass
    def test___ge__(self):
        """What does </> even mean for a dict? Skip."""
        pass
    def test___getattribute__(self):
        """Inherited from <object>. Skip."""
        pass
    def test___getitem__(self):
        """dict.__getitem__() is equivalent to map.__getitem__()"""
        for k in self.dict.keys():
            self.assertEqual(self.dict.__getitem__(k), self.dict.__getitem__(k))
        pass
    def test___gt__(self):
        """What does </> even mean for a dict? Skip."""
        pass
    def test___hash__(self):
        """Not strictly speaking equal to each other. Skip."""
        pass
    def test___init__(self):
        """map accepts the same constructors as dict"""
        newMap = self.mapClass()
        items = newMap.items()
        self.assertEqual(list(items), [])
        newMap = self.mapClass(self.dict)
        self.assertEqual(sorted(list(newMap.items())), sorted(list(self.dict.items())))
        newMap = self.mapClass(list(self.dict.items()))
        self.assertEqual(sorted(newMap.items()), sorted(self.dict.items()))

        pass
    def test___iter__(self):
        """dict.__iter__() is equivalent to map.__iter__()"""
        self.assertEqual(list(self.map), sorted(self.dict))
        pass
    def test___le__(self):
        """What does </> even mean for a dict? Skip."""
        pass
    def test___len__(self):
        """dict.__len__() is equivalent to map.__len__()"""
        self.assertEqual(self.dict.__len__(), self.map.__len__())
        pass
    def test___lt__(self):
        """What does </> even mean for a dict? Skip."""
        pass
    def test___ne__(self):
        """Not implementing comparison for now. Skip."""
        pass
    def test___new__(self):
        """Inherited from <object>. Skip."""
        pass
    def test___reduce__(self):
        """Don't support pickling for now. Skip."""
        pass
    def test___reduce_ex__(self):
        """Don't support pickling for now. Skip."""
        pass
    def test___repr__(self):
        """Don't care how it prints. Skip."""
        pass
    def test___setattr__(self):
        """Inherited from <object> Skip."""
        pass
    def test___setitem__(self):
        """dict.__setitem__() is equivalent to map.__setitem__()"""
        k = 'a new key!'; v = 'a new value!'
        self.assertEqual(k in self.dict, False)
        self.assertEqual(k in self.map, False)
        self.dict.__setitem__(k,v)
        self.map.__setitem__(k,v)
        self.assertEqual(self.map[k], self.dict[k])
        pass
    def test___str__(self):
        """Don't care how it prints. Skip."""
        pass
    def test_clear(self):
        """dict.clear() is equivalent to map.clear()"""
        self.assertEqual(self.dict.clear(), self.map.clear())
        self.assertEqual([k for k,v in self.dict], [])
        self.assertEqual([k for k,v in self.map], [])
        pass
    def test_get(self):
        """dict.get() is equivalent to map.get()"""
        default = 42
        key = list(self.dict.keys())[0]
        nokey = 'i am not a key'
        self.assertEqual(self.map[key], self.map.get(key,default))
        self.assertEqual(default, self.map.get(nokey,default))
        self.assertEqual(None, self.map.get(nokey))


        pass
    def test_items(self):
        """dict.items() is equivalent to map.items()"""
        dictItems = sorted(self.dict.items())
        mapItems = sorted(self.map.items())
        self.assertEqual(dictItems, mapItems)
        pass
    def test_keys(self):
        """dict.keys() is equivalent to map.keys()"""
        dictItems = sorted(self.dict.keys())
        mapItems = sorted(self.map.keys())
        self.assertEqual(dictItems, mapItems)

        self.assertEqual(len(self.dict.keys()), len(self.map.keys()))

        self.assertEqual(self.map.keys(), self.dict.keys())
        self.assertNotEqual(self.dict.keys(), 1)
        self.assertNotEqual(self.map.keys(), 1)

        self.assertNotEqual(
            self.map.keys(),
            [list(self.map.values())[0]]*len(self.map),
            "should compare nonequal, even if same length and all items contained"
        )

        mkv = self.map.keys()
        # __contains__ should not exhaust the view like it would for an iterator
        for _ in range(2):
            for k in reversed(self.dict.keys()):
                assert k in mkv

        # our views aren't reversible
        dkv = self.dict.keys()
        with self.assertRaises(TypeError):
            for k in reversed(self.map.keys()):
                assert k in dkv

        if sys.version_info >= (3,10):
            self.assertEqual(dict(mkv.mapping), dkv.mapping)

    def test_pop(self):
        """dict.pop() is equivalent to map.pop()"""
        default = 42
        key = list(self.dict.keys())[0]
        value = self.dict[key]
        nokey = 'i am not a key'
        self.assertEqual(self.map.pop(key,default), value)
        self.assertEqual(self.map.pop(key,default), default)
        self.assertRaises(KeyError,self.map.pop,key)
        self.assertRaises(KeyError,self.map.__getitem__,key)

        pass
    def test_popitem(self):
        """dict.popitem() is equivalent to map.popitem()"""
        items = [self.map.popitem() for i in range(len(self.map))]
        self.assertEqual(sorted(items), sorted(self.dict.items()))
        self.assertEqual(len(self.map), 0)
        self.assertRaises(KeyError,self.map.popitem)
        pass
    def test_setdefault(self):
        """The template would have to know a default value for each type. Skip."""
        pass
    def test_update(self):
        """dict.update() is equivalent to map.update()"""
        # only half-support update(self, dict, **kwargs):
        # can't add kwargs to map because of typing issues
        modpair = (list(self.dict.keys())[0],'a new value!')
        newpair = ('nogat','stuebl')
        oldkeys = list(self.dict.keys())[1:]
        newdict = {modpair[0]:modpair[1], newpair[0]:newpair[1]}
        self.map.update(newdict)
        for key in oldkeys:
            self.assertEqual(self.map[key], self.dict[key])
        for key in (modpair[0],newpair[0]):
            self.assertEqual(self.map[key], newdict[key])
        pass
    def test_values(self):
        """dict.values() is equivalent to map.values()"""
        self.assertEqual(sorted(self.map.values()), sorted(self.dict.values()))
        # values views are never equal to anything
        self.assertNotEqual(self.map.values(), self.dict.values())
        self.assertNotEqual(self.dict.values(), self.map.values())
        self.assertNotEqual(self.dict.values(), self.dict.values())

    def test_view_references(self):
        m = dataclasses.I3MapIntVectorInt({0: []})
        for v in m.values():
            v.append(1)
        self.assertListEqual([list(v) for v in m.values()], [[1]])
        for k, v in m.items():
            v.append(2)
        self.assertEqual([list(v) for v in m.values()], [[1, 2]])

    def test_view_dynamism(self):
        m = icetray.map_int_int({i: i for i in range(5,10)})
        # begin() currently points to 5
        keys = m.keys()
        self.assertEqual(len(list(keys)), 5)
        m.update({i: i for i in range(2)})
        # invalidate the result of the previous begin()
        del m[5]
        # nothing blows up, as iteration invokes begin() again 
        self.assertEqual(len(list(keys)), 6)

if __name__ == '__main__':
    unittest.main()
