#!/usr/bin/env python

import unittest
from icecube import dataclasses, dataio
from I3Tray import I3Units
import sys, os

class TestI3MCTreeUtils(unittest.TestCase):

	def test_I3MCTreeUtils(self):
		inFile = dataio.I3File(os.path.expandvars('$I3_TESTDATA/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2'))
		frame = inFile.pop_physics()
		mctree0 = frame['I3MCTree']
		frame = inFile.pop_physics()
		mctree1 = frame['I3MCTree']
		self.assertEqual(mctree0.has(mctree0[0]),True,"must be euqal")
		self.assertEqual(mctree0.has(mctree1[0]),False,"must be euqal")
		inFile.close()


unittest.main()
