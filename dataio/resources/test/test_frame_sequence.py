#!/usr/bin/env python

from icecube import icetray, dataio

import os
import tempfile
import shutil
import unittest

class TestFrameSequence(unittest.TestCase):

    def setUp(self):
        tmpdir = tempfile.mkdtemp()
        def cleanup():
            shutil.rmtree(tmpdir)
        self.addCleanup(cleanup)

        self.name = os.path.join(tmpdir,'f1.i3')
        f = dataio.I3File(self.name,'w')
        f.push(icetray.I3Frame(icetray.I3Frame.Geometry))
        f.push(icetray.I3Frame(icetray.I3Frame.Calibration))
        f.push(icetray.I3Frame(icetray.I3Frame.DetectorStatus))
        f.push(icetray.I3Frame(icetray.I3Frame.DAQ))
        f.push(icetray.I3Frame(icetray.I3Frame.Physics))
        f.push(icetray.I3Frame(icetray.I3Frame.DAQ))
        f.push(icetray.I3Frame(icetray.I3Frame.Physics))
        f.close()

    def test_01(self):
        # closed file
        f = dataio.I3FrameSequence([self.name])
        f.close()
        if f.more():
            raise Exception('should not be able to more() a closed file')

    def test_02(self):
        # test pop_frame for a non-reading file
        f = dataio.I3FrameSequence([self.name])
        f.close()

        try:
            f.pop_frame()
        except Exception:
            pass
        else:
            raise Exception('should not be able to pop_frame() on a non-reading file')

    def test_03(self):
        # get mixed frames
        f = dataio.I3FrameSequence([self.name])
        f.pop_frame()
        f.pop_frame()
        f.pop_frame()
        f.pop_frame()
        fr = f.get_mixed_frames()
        self.assertEqual(len(fr), 3, 'not 3 mixed frames')
        if fr[0].Stop != icetray.I3Frame.Geometry:
            raise Exception('no mixed Geometry frame')
        if fr[1].Stop != icetray.I3Frame.Calibration:
            raise Exception('no mixed Calibration frame')
        if fr[2].Stop != icetray.I3Frame.DetectorStatus:
            raise Exception('no mixed DetectorStatus frame')
        f.close()

    def test_04(self):
        # test iter
        f = dataio.I3FrameSequence([self.name])
        iter = f.__iter__()
        iter2 = iter.__iter__()
        f.rewind()
        for fr in f:
            pass
        f.close()

    def test_05(self):
        # test direct index
        f = dataio.I3FrameSequence([self.name])
        if f[0].Stop != icetray.I3Frame.Geometry:
            raise Exception('no Geometry frame')
        if f[1].Stop != icetray.I3Frame.Calibration:
            raise Exception('no Calibration frame')
        if f[2].Stop != icetray.I3Frame.DetectorStatus:
            raise Exception('no DetectorStatus frame')
        f.close()

    def test_06(self):
        n_frames = 0
        with dataio.I3File(self.name) as f:
            while f.more():
                n_frames += 1
                f.pop_frame()
        self.assertEqual(n_frames, 7, 'wrong number of frames')

        # now test an already open file
        n_frames = 0
        f1 = dataio.I3File(self.name)
        with f1 as f:
            while f.more():
                n_frames += 1
                f.pop_frame()
        self.assertEqual(n_frames, 7, 'wrong number of frames')

        try:
            f1.pop_frame()
        except RuntimeError:
            pass
        else:
            raise Exception('pop_frame on closed file worked')


if __name__ == '__main__':
    unittest.main()
