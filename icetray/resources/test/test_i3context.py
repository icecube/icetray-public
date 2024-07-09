#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import icetray

class test_I3Context(unittest.TestCase):

    def setUp(self):
        self.ctx = icetray.I3Context()
        self.ctx['fun'] = lambda foo : True

    def test_pybindings(self):
        print("%s" % str(self.ctx))

        self.assertEqual(len(self.ctx), 1,
                         "wrong number of items in the context.")

        self.assertEqual(len(self.ctx.items()), 1,
                         "wrong number of items in the context.")

        self.assertEqual(len([i for i in self.ctx.items()]), 1,
                         "wrong number of items in the context.")

        self.assertEqual(len([v for v in self.ctx.values()]), 1,
                         "wrong number of items in the context.")

unittest.main()




