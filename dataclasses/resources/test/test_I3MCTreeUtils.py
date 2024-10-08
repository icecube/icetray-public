#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses, dataio
from icecube.icetray import I3Units
import sys, os

class TestI3MCTreeUtils(unittest.TestCase):

    def test_I3MCTreeUtils(self):
        inFile = dataio.I3File(os.path.expandvars('$I3_TESTDATA/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2'))
        frame = inFile.pop_physics()
        mctree0 = frame['I3MCTree']
        frame = inFile.pop_physics()
        mctree1 = frame['I3MCTree']
        self.assertEqual(mctree0.has(mctree0[0]),True,"must be equal")
        self.assertEqual(mctree0.has(mctree1[0]),False,"must be equal")
        inFile.close()


unittest.main()
