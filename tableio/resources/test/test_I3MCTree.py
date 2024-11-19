#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Ensure that I3MCTree and I3LinearizedMCTree can both be written
"""

import os
import unittest

from icecube import phys_services  # noqa: F401
from icecube import dataclasses, icetray
from icecube.icetray import I3Tray

icetray.logging.set_level_for_unit("I3Tray", "WARN")

def add_eventheader(frame):
    eh = dataclasses.I3EventHeader()
    eh.run_id = 1
    eh.event_id = add_eventheader.event_id
    add_eventheader.event_id += 1
    frame['I3EventHeader'] = eh
add_eventheader.event_id = 1

def make_mctree(frame, linearized=True):
    tree = dataclasses.I3MCTree()
    p1 = dataclasses.I3Particle()
    tree.insert(p1)
    p2 = dataclasses.I3Particle()
    tree.append_child(p1, p2)
    if linearized:
        tree = dataclasses.I3LinearizedMCTree(tree)
    frame['I3MCTree'] = tree

def try_to_write(file=os.path.basename(__file__)+".hdf5",
                 linearized=False):

    try:
        from icecube.hdfwriter import I3HDFWriter
    except ImportError:
        raise unittest.SkipTest("hdfwriter project missing")

    tray = I3Tray()

    tray.Add('I3InfiniteSource')
    tray.Add(add_eventheader, Streams=[icetray.I3Frame.DAQ])
    tray.Add("I3NullSplitter", "nullsplit")

    tray.Add(make_mctree, linearized=linearized)
    tray.Add(I3HDFWriter,
        Output=file,
        Keys=['I3MCTree'],
        Types=[],
        SubEventStreams=["nullsplit"],
    )

    tray.Execute(1)


class MCTreeTest(unittest.TestCase):
    test_output = os.path.basename(__file__) + ".hdf5"
    def setUp(self):
        try:
            import h5py
        except ImportError:
            raise unittest.SkipTest("h5py missing")

        if 'File' not in dir(h5py):
            raise unittest.SkipTest("h5py missing or incomplete")

        try_to_write()
    def tearDown(self):
        os.unlink(self.test_output)
    def testNumberOfRows(self):
        import h5py
        with h5py.File(self.test_output,'r') as hdf:
            self.assertIsNotNone(hdf['/I3MCTree'], "I3MCTree table exists")
            self.assertEqual(len(hdf['/I3MCTree']), 2, "I3MCTree table has 2 rows")

class LinearizedMCTreeTest(MCTreeTest):
    def setUp(self):
        try:
            import h5py
        except ImportError:
            raise unittest.SkipTest("pytables missing")

        if 'File' not in dir(h5py):
            raise unittest.SkipTest("pytables missing or incomplete")

        try_to_write(linearized=True)
    def tearDown(self):
        os.unlink(self.test_output)
    def testNumberOfRows(self):
        import h5py
        with h5py.File(self.test_output,'r') as hdf:
            self.assertIsNotNone(hdf['/I3MCTree'], "I3MCTree table exists")
            self.assertEqual(len(hdf['/I3MCTree']), 2, "I3MCTree table has 2 rows")

if __name__ == "__main__":
    unittest.main(verbosity=2)
