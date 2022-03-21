#!/usr/bin/env python3

from icecube import icetray, dataio
from icecube.icetray.i3logging import *
icetray.logging.set_level(icetray.logging.I3LogLevel.LOG_DEBUG)

import os
import tempfile
import shutil
import unittest

class TestFrameSequence(unittest.TestCase):

    def setUp(self):
        log_debug("Setting up test...")
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
        log_debug("Executing test_01...")
        # closed file
        f = dataio.I3FrameSequence([self.name])
        f.close()
        if f.more():
            raise Exception('should not be able to more() a closed file')
        log_debug("Finished test_01.")

    def test_02(self):
        log_debug("Executing test_02...")
        # test pop_frame for a non-reading file
        f = dataio.I3FrameSequence([self.name])
        f.close()

        try:
            f.pop_frame()
        except Exception:
            pass
        else:
            raise Exception('should not be able to pop_frame() on a non-reading file')

        log_debug("Finished test_02.")

    def test_03(self):
        log_debug("Executing test_03...")
        # get mixed frames
        log_debug("Creating frame sequence...")
        f = dataio.I3FrameSequence([self.name])
        log_debug("Popping frame (1/4)...")
        f.pop_frame()
        log_debug("Popping frame (2/4)...")
        f.pop_frame()
        log_debug("Popping frame (3/4)...")
        f.pop_frame()
        log_debug("Popping frame (1/4)...")
        f.pop_frame()
        log_debug("Looking for mixed frames...")
        fr = f.get_mixed_frames()
        self.assertEqual(len(fr), 3, 'not 3 mixed frames')
        log_debug("Checking frame stops...")
        if fr[0].Stop != icetray.I3Frame.Geometry:
            raise Exception('no mixed Geometry frame')
        if fr[1].Stop != icetray.I3Frame.Calibration:
            raise Exception('no mixed Calibration frame')
        if fr[2].Stop != icetray.I3Frame.DetectorStatus:
            raise Exception('no mixed DetectorStatus frame')
        log_debug("Closing frame sequence...")
        f.close()
        log_debug("Finished test_03.")

    def test_04(self):
        log_debug("Executing test_04...")
        # test iter
        log_debug("Creating frame sequence...")
        f = dataio.I3FrameSequence([self.name])
        log_debug("Iterating over frame sequence")
        iter = f.__iter__()
        iter2 = iter.__iter__()
        log_debug("Iterating in reverse")
        f.rewind()
        for fr in f:
            pass
        log_debug("Closing frame sequence...")
        f.close()
        log_debug("Finished test_04.")

    def test_05(self):
        log_debug("Executing test_05...")
        # test direct index
        log_debug("Creating frame sequence...")
        f = dataio.I3FrameSequence([self.name])
        log_debug("Checking frame stops...")
        if f[0].Stop != icetray.I3Frame.Geometry:
            raise Exception('no Geometry frame')
        if f[1].Stop != icetray.I3Frame.Calibration:
            raise Exception('no Calibration frame')
        if f[2].Stop != icetray.I3Frame.DetectorStatus:
            raise Exception('no DetectorStatus frame')
        log_debug("Closing frame sequence...")
        f.close()
        log_debug("Finished test_05.")

    def test_06(self):
        log_debug("Executing test_06...")
        n_frames = 0
        log_debug("Checking newly opened file")
        with dataio.I3File(self.name) as f:
            while f.more():
                n_frames += 1
                f.pop_frame()
        self.assertEqual(n_frames, 7, 'wrong number of frames')

        # now test an already open file
        n_frames = 0
        log_debug("Checking previously opened file")
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

        log_debug("Finished test_06.")

if __name__ == '__main__':
    unittest.main()
