#! /usr/bin/env python

import sys
import os
import unittest

from icecube import dataio
from icecube import dataclasses
from icecube import icetray

# This file is primarily a test of the fileadaptor class,
# which is used in pyshovel
from icecube.dataio import fileadaptor

test_filename = "adaptor_test.i3"

test_frames = [('a', 0),
               ('b', 1),
               ('c', 2),
               ('d', 3),
               ('e', 4),
               ('f', 5),
               ('g', 6),
               ('h', 7)]

def setUpModule():
    """
     Generate .i3 file (borrowed from dataio test_randomaccess_file)
    """
    i3f = dataio.I3File(test_filename, dataio.I3File.Mode.Writing)
    frames = test_frames

    for pr in frames:
        frame = icetray.I3Frame(icetray.I3Frame.Stream(pr[0]))
        frame[pr[0]] = icetray.I3Int(pr[1])
        i3f.push(frame)

    i3f.close()

def tearDownModule():
    os.unlink(test_filename)


class AdapatorTest(unittest.TestCase):
    def setUp(self):
        self.adapt = fileadaptor.I3FileAdaptor(test_filename)

    def test_len(self):
        self.assertEqual(len(self.adapt), 8)

    def test_iterator(self):
        for i, f in enumerate(self.adapt):
            self.assertEqual(f.Stop.id, self.adapt[i].Stop.id)
        # Also test independence of iterators.
        # The following would fail if using I3File.__iter__ directly, per [IT340]
        i1 = iter(self.adapt)
        next(i1)
        next(i1)
        i2 = iter(self.adapt)
        self.assertEqual(next(i1).Stop.id, test_frames[2][0])
        self.assertEqual(next(i2).Stop.id, test_frames[0][0])

    def test_basic_indexing(self):
        for x in range(8):
            frame = self.adapt[x]
            self.assertEqual(frame.Stop.id, test_frames[x][0])

    def test_negative_indexing(self):
        for x in range(8):
            idx = -8+x
            frame = self.adapt[idx]
            self.assertEqual(frame.Stop.id, test_frames[x][0])
        # also check negative zero
        self.assertEqual(self.adapt[0].Stop.id, self.adapt[-0].Stop.id)

    def test_basic_slicing(self):
        test_stop_ids = [f[0] for f in test_frames]
        slice_indices = [(0,3), (1,5), (6,8), (6,12), (2,-2)]

        for start, stop in slice_indices:
            frames = self.adapt[start:stop]
            self.assertEqual([f.Stop.id for f in frames], test_stop_ids[start:stop])

    def test_fancy_slicing(self):
        test_stop_ids = [f[0] for f in test_frames]
        slice_indices = [(0,3,2), (None,None,-1), (-2, -4, -1)]
        for start, stop, step in slice_indices:
            frames = self.adapt[start:stop:step]
            self.assertEqual([f.Stop.id for f in frames], test_stop_ids[start:stop:step])

    def test_bogus_indexing(self):
        self.assertRaises(IndexError, self.adapt.__getitem__, -9)
        self.assertRaises(IndexError, self.adapt.__getitem__, 9)
        self.assertRaises(IndexError, self.adapt.__getitem__, "foo")
        self.assertRaises(IndexError, self.adapt.__getitem__, [12])

#@unittest.skip("Skip until implemented")
class SequentialAdaptorTest(AdapatorTest):

    def setUp(self):
        self.adapt = fileadaptor.I3FileAdaptor(dataio.I3File(test_filename))

class SequentialAdaptorSmallCacheTest(AdapatorTest):

    def setUp(self):
        self.adapt = fileadaptor.I3SequentialAdaptor(dataio.I3File(test_filename), cachesize=2)

if __name__ == "__main__":
    # In Python 2.7 and later, the unittest framework is smart enough to run
    # setUpModule() and tearDownModule() automatically.  For the sake of older
    # pythons, call them manually here.  They'll be invoked twice under 2.7,
    # which is fine.
    setUpModule()
    unittest.main()
    tearDownModule()
