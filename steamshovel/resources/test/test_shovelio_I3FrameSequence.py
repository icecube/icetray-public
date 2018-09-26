#!/usr/bin/env python
# -*- coding: utf-8 -*-
from icecube import icetray
from icecube.shovelio import I3FrameSequence
from icecube.dataio import I3File
import unittest
import os
import time
import sys

# icetray.set_log_level_for_unit("shovelio", icetray.I3LogLevel.LOG_TRACE)

e = os.path.expandvars
seekable_file = e("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")

class ShovelIoTest(unittest.TestCase):

    def setUp(self):
        ref = I3File(seekable_file)

        self.ref_frame_info = []
        while ref.more():
            frame = ref.pop_frame()
            self.ref_frame_info.append({
                "stop": frame.Stop,
                "keys": set(frame.keys())
            })

        for x in self.ref_frame_info:
            print(x["stop"], len(x["keys"]))

    def tearDown(self):
        del self.ref_frame_info

    def testForwardAccess(self):
        seq = I3FrameSequence()
        seq.add_file(seekable_file)
        while seq.is_indexing:
            time.sleep(0.1)

        # skip TrayInfo
        for i, ref in enumerate( self.ref_frame_info ):
            cur = seq[i]
            self.assertEqual(ref["stop"], cur.Stop, "idx {0}: stops differ, {1} vs {2}".format(i, ref["stop"], cur.Stop))
            self.assertEqual(ref["keys"], set(cur.keys()),
                             "idx {0}: keys differ\nref: {1}\ntst: {2}".format(i, ref["keys"], set(cur.keys())))

    def testReverseAccess(self):
        seq = I3FrameSequence()
        seq.add_file(seekable_file)
        while seq.is_indexing:
            time.sleep(0.1)

        # skip TrayInfo
        for i in range( len(self.ref_frame_info) - 1, 0, -1 ):
            cur = seq[i]
            ref = self.ref_frame_info[i]
            self.assertEqual(ref["stop"], cur.Stop, "idx {0}: stops differ, {1} vs {2}".format(i, ref["stop"], cur.Stop))
            self.assertEqual(ref["keys"], set(cur.keys()), "idx {0}: keys differ".format(i))

if __name__ == '__main__':
    unittest.main()
