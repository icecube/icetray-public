# -*- coding: utf-8 -*-
# 
# copyright  (C) 2011
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
#

from icecube.icetray import I3Module
import unittest

def I3TestModuleFactory(*test_cases):
	"""
Create an I3Module that runs the given unittest test cases on each Physics frame.

Usage:

class I3TimeHorizonCutTest(unittest.TestCase):
	def testKeys(self):
		\"""All keys are present.""\"
		self.assertTrue('InIceRawData' in self.frame, "Up is down, black is white!")
		self.assertTrue('InIceRawData_Horizon' in self.frame, "Up is down, black is white!")
		
	def testSizeReduction(self):
		\"""Output map is ever so slightly smaller""\"
		inlaunches = self.frame['InIceRawData']
		outlaunches = self.frame['InIceRawData_Horizon']
		self.assert_(len(outlaunches) <= len(inlaunches))
		
tray.AddModule(I3TestModuleFactory(I3TimeHorizonCutTest), 'testify')

	"""
	class I3TestModule(I3Module):
		runner = unittest.TextTestRunner()
		def __init__(self, context):
			I3Module.__init__(self, context)
			self.AddOutBox("OutBox")
			
			self.suites = []
			for case in test_cases:
				self.suites.append(unittest.defaultTestLoader.loadTestsFromTestCase(case))
			self.test_cases = test_cases
			
		def Configure(self):
			pass
		
		def Physics(self, frame):
			for case in test_cases:
				case.frame = frame
			for test in self.suites:
				self.runner.run(test)
			self.PushFrame(frame)
		
		def Finish(self):
			pass
	
	return I3TestModule
