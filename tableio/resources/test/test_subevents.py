#!/usr/bin/env python

# Ensure that row padding works properly with split streams.

from icecube import icetray, dataclasses, dataio, tableio, phys_services
import I3Tray
import os, sys, random, unittest

try:
	from icecube import hdfwriter
	import tables
except ImportError:
	sys.exit()
	
def headerfaker(frame):
	header = dataclasses.I3EventHeader()
	header.run_id = 0
	header.event_id = headerfaker.event
	headerfaker.event += 1
	frame['I3EventHeader'] = header
headerfaker.event = 0

def emitter(frame, label, prob=0.5):
	if (random.random() < prob):
		particle = dataclasses.I3Particle()
		frame[label] = particle

def streampick(stream):
	def pick(frame):
		return frame.Stop != icetray.I3Frame.Physics or frame['I3EventHeader'].sub_event_stream == stream
	return pick


class SubeventTest(unittest.TestCase):
	fname = os.environ['I3_BUILD'] + '/hdfwriter/subevent_test.hdf5'
	@classmethod
	def runtray(cls, fname):
		tray = I3Tray.I3Tray()
		
		tray.AddModule("I3InfiniteSource", "source", stream=icetray.I3Frame.DAQ)
		
		tray.AddModule(headerfaker, 'headers', Streams=[icetray.I3Frame.DAQ])
		
		tray.AddModule("I3NullSplitter", "s1")
		tray.AddModule("I3NullSplitter", "s2")
		
		for i in range(10):
			tray.AddModule(emitter, 's1e%d' % i, label='s1e%d' % i, prob=0.1, If=streampick("s1"))
			
		for i in range(10):
			tray.AddModule(emitter, 's2e%d' % i, label='s2e%d' % i, prob=0.1, If=streampick("s2"))

		tabler = hdfwriter.I3HDFTableService(fname)
		tray.AddModule(tableio.I3TableWriter, 'scribe',
			tableservice=tabler,
			types=[dataclasses.I3Particle],
			SubEventStreams=['s1','s2'],
			)
		
		
		tray.Execute(100)
		
	def setUp(self):
		self.__class__.runtray(self.fname)
	def tearDown(self):
		os.unlink(self.fname)
	def testRowAlignment(self):
		hdf = tables.open_file(self.fname)
		tabs = []
		for i in range(10):
			for j in range(2):
				try:
					tabs.append(hdf.get_node('/s%de%d' % (j,i)))
				except:
					pass
		nrows = tabs[0].nrows
		for tab in tabs[1:]:
			self.assertEquals(nrows, tab.nrows)
		for i in range(nrows):
			canonical = tabs[0][i]
			for tab in tabs[1:]:
				row = tab[i]
				for field in ['Run', 'Event', 'SubEvent', 'SubEventStream']:
					self.assertEquals(canonical[field], row[field],
					    "'%s' are equal in row %d (%d != %d)" % (field, i, 
					    canonical[field], row[field]))
		hdf.close()

class SubeventMergingTest(unittest.TestCase):
	fname1 = os.environ['I3_BUILD'] + '/hdfwriter/subevent_test_1.hdf5'
	fname2 = os.environ['I3_BUILD'] + '/hdfwriter/subevent_test_2.hdf5'
	fname_merged = os.environ['I3_BUILD'] + '/hdfwriter/subevent_test_merged.hdf5'
	def setUp(self):
		SubeventTest.runtray(self.fname1)
		SubeventTest.runtray(self.fname2)
		from subprocess import call
		call([os.environ['I3_BUILD'] + "/hdfwriter/resources/scripts/merge.py", "-o", self.fname_merged, self.fname1, self.fname2])
	def tearDown(self):
		for f in [self.fname1, self.fname2, self.fname_merged]:
			os.unlink(f)
	def testMergedAlignment(self):
		hdf1 = tables.open_file(self.fname1)
		hdf2 = tables.open_file(self.fname2)
		hdfmerge = tables.open_file(self.fname_merged)

		tabs = []
		for i in range(10):
			for j in range(2):
				try:
					tabs.append(hdf1.get_node('/s%de%d' % (j,i)))
				except:
					pass
		nrows1 = tabs[0].nrows
		tabs = []
		for i in range(10):
			for j in range(2):
				try:
					tabs.append(hdf2.get_node('/s%de%d' % (j,i)))
				except:
					pass
		nrows2 = tabs[0].nrows
		for tab in tabs:
			itab = hdf2.get_node('/__I3Index__/%s' % tab.name)
			imtab = hdfmerge.get_node('/__I3Index__/%s' % tab.name)
			mtab = hdfmerge.get_node('/%s' % tab.name)
			self.assertEquals(mtab.nrows, nrows1+nrows2)
			for i in range(nrows2):
				row = tab[i]
				mrow = mtab[i+nrows1]
				for field in ['Run', 'Event', 'SubEvent', 'SubEventStream', 'exists']:
					self.assertEquals(row[field], mrow[field],
					    "'%s' are equal in row %d (%d != %d)" % (field, i, 
					    row[field], mrow[field]))
				irow = itab[i]
				imrow = imtab[i+nrows1]
				for field in ['Run', 'Event', 'SubEvent', 'SubEventStream', 'exists']:
					self.assertEquals(irow[field], imrow[field],
					    "'%s' are equal in row %d (%d != %d)" % (field, i, 
					    irow[field], imrow[field]))
		hdf1.close()
		hdf2.close()
		hdfmerge.close()

if __name__ == "__main__":
	unittest.main()
	

