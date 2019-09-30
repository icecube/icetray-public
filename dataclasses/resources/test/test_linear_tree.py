#!/usr/bin/env python

import sys, os
infiles = sys.argv[1:]

if len(infiles) == 0:
	infiles = [os.path.expandvars('$I3_TESTDATA/dataclasses/9036_coinc.i3.bz2')]

import unittest, math
from icecube import icetray, dataio, dataclasses
from icecube.dataclasses import I3LinearizedMCTree, I3MCTree

class LinearTreeTest(unittest.TestCase):
	def setUp(self):
		self.mctree = self.frame['I3MCTree']
		self.re_mctree = self.frame['I3LinearizedMCTree']
	def testSize(self):
		self.assertEquals(len(self.mctree), len(self.re_mctree))
	
	def assertEquivalent(self, d1, d2, tol=None):
		"""One-stop shop for asserting equality to within a tolerance"""
		if math.isnan(d1) and math.isnan(d2):
			pass
		else:
			if tol is None:
				self.assertEquals(d1, d2)
			elif abs(d1-d2) >= tol:
				raise AssertionError("|%s - %s| >= %s" % (d1, d2, tol))
	def assertEqualParticle(self, p1, p2):
		self.assertEquivalent(p1.dir.zenith, p2.dir.zenith)
		self.assertEquivalent(p1.dir.azimuth, p2.dir.azimuth)
		self.assertEquivalent(p1.pos.x, p2.pos.x, 5e-1)
		self.assertEquivalent(p1.pos.y, p2.pos.y, 5e-1)
		self.assertEquivalent(p1.pos.z, p2.pos.z, 5e-1)
		self.assertEquivalent(p1.time, p2.time, 1e-2)
		
		self.assertEquivalent(p1.energy, p2.energy, 1e-2)
		self.assertEquivalent(p1.length, p2.length)
		self.assertEquivalent(p1.speed, p2.speed)
		
		self.assertEquals(p1.type, p2.type)
		self.assertEquals(p1.location_type, p2.location_type)
		self.assertEquals(p1.shape, p2.shape)
		self.assertEquals(p1.fit_status, p2.fit_status)
		
		self.assertEquals(p1.major_id, p2.major_id)
		self.assertEquals(p1.minor_id, p2.minor_id)
		
		
	def testEquivalence(self):
		
		for i, (raw, reco) in enumerate(zip(self.mctree, self.re_mctree)):
			try:
				self.assertEqualParticle(raw, reco)
			except AssertionError:
				print(i)
				print(raw)
				print(reco)
				print(self.mctree)
				print(self.re_mctree)
				raise
			
			
from I3Tray import I3Tray

# first round: convert and serialize
tray = I3Tray()
outfile = 'i3linearizedmctree_tmp.i3.bz2'
tray.AddModule('I3Reader', 'reader', filenamelist=infiles)
tray.AddModule('Keep', 'keeper', Keys=['I3MCTree'])
# force re-serialization of original I3MCTree to ensure that particle IDs
# in the tree are unique
def clone(frame):
	mctree = frame['I3MCTree']
	del frame['I3MCTree']
	frame['I3MCTree'] = mctree
	frame['I3LinearizedMCTree'] = I3LinearizedMCTree(mctree)
tray.Add(clone, Streams=[icetray.I3Frame.DAQ])

tray.AddModule('I3Writer', 'writer',
    Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
    # DropOrphanStreams=[icetray.I3Frame.DAQ],
    filename=outfile)    


tray.Execute(100)


# second round: read back and test
tray = I3Tray()
tray.AddModule('I3Reader', 'reader', filenamelist=[outfile])

tray.AddModule(icetray.I3TestModuleFactory(LinearTreeTest), 'testy',
    Streams=[icetray.I3Frame.DAQ])
    
tray.AddModule('Delete', 'kill_mctree', Keys=['I3MCTree'])

outfile = 'i3linearizedmctree_compact.i3.bz2'
tray.AddModule('I3Writer', 'writer',
    Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
    # DropOrphanStreams=[icetray.I3Frame.DAQ],
    filename=outfile)    

tray.Execute(100)

