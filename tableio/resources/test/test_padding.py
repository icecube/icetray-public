#!/usr/bin/env python3

"""
Ensure that hdf5 write does padding correctly for S and non-S frames
"""

import os
import re
from icecube import icetray, dataclasses, tableio, phys_services, hdfwriter
from I3Tray import I3Tray

have_simclasses = False
try:
    from icecube import simclasses
    have_simclasses = True 
except ImportError:
    pass

import sys
import unittest

class Generator(icetray.I3Module):
    def __init__(self,context):
        icetray.I3Module.__init__(self, context)
        self.counter=0

    def Process(self):

        if have_simclasses and self.counter%20==0:
            sframe=icetray.I3Frame('S')
            info = simclasses.I3CorsikaInfo()
            sframe['I3CorsikaInfo']=info
            self.PushFrame(sframe)
        
        qframe=icetray.I3Frame('Q')
        eh = dataclasses.I3EventHeader()
        eh.run_id = 1
        eh.event_id = self.counter
        qframe['I3EventHeader']=eh
        
        tree = dataclasses.I3MCTree()
        p1 = dataclasses.I3Particle()        
        tree.insert(p1)

        if self.counter%2==0:
            qframe['I3MCTree'] = tree
            qframe['I3MCPrimary'] = p1
        
        self.PushFrame(qframe)
        self.counter+=1

        if self.counter>=100:
            self.RequestSuspension()

class MCTreeTest(unittest.TestCase):
    def setUp(self):

        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")

        if 'open_file' not in dir(tables):
            raise unittest.SkipTest("pytables missing or incomplete")

        tray = I3Tray()
        tray.Add(Generator)
        tray.Add("I3NullSplitter", "nullsplit",SubEventStreamName='null')
        tray.Add(hdfwriter.I3HDFWriter,
                 Output='foo.hdf5',
                 Keys=['I3CorsikaInfo','I3EventHeader','I3MCTree','I3MCPrimary'],
                 Types=[],
                 SubEventStreams=["null"],
        )
        tray.Execute()

    def tearDown(self):
        os.unlink("foo.hdf5")
        pass

    def testNumberOfRows(self):
        import tables
        hdf = tables.open_file('foo.hdf5')

        if have_simclasses:
            self.assertEqual(len(hdf.root.I3CorsikaInfo),5,"I3CorsikaInfo should have 5 entries")
        self.assertEqual(len(hdf.root.I3MCTree),50,"I3MCTree should have 50 entries")
        self.assertEqual(len(hdf.root.I3MCPrimary),100,"I3MCPrimary should have 100 entries")

if __name__ == "__main__":
    unittest.main(verbosity=2)
