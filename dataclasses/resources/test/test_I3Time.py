#!/usr/bin/env python
"""
I3Time tests
"""

import unittest

from icecube.dataclasses import I3Time


class I3TimeTest(unittest.TestCase):
    def test_print(self):
        t = I3Time(2012,116640000117197235)
        print(t)
        print(t.__repr__())
        
        s = str(t)
        subsecond = int(s.split(' ')[-2].split('.')[-1].replace(',',''))
        self.assertEqual(subsecond,117197235)

if __name__ == '__main__':
    unittest.main()
