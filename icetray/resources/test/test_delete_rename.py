#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import icetray
from icecube.icetray import I3Tray

# we're expecting LOG_ERROR, don't need to log them
icetray.set_log_level_for_unit("I3Tray", icetray.I3LogLevel.LOG_FATAL)


class test_I3FrameObject_delete_rename(unittest.TestCase):
    def setUp(self):
        self.tray = I3Tray()

        def add_frame_object(frame):
            # any frame object will do
            frame["FrameObject"] = icetray.OMKey(21, 30)

        self.tray.Add("BottomlessSource")
        self.tray.Add(add_frame_object)

    @unittest.expectedFailure
    def test_delete_rename(self):
        self.tray.Add("Delete", keys="FrameObject")
        self.tray.Add("Rename", keys=["FrameObject", "OMKey"])
        self.tray.Execute(10)

    @unittest.expectedFailure
    def test_rename_delete(self):
        self.tray.Add("Rename", keys=["FrameObject", "OMKey"])
        self.tray.Add("Delete", keys="FrameObject")
        self.tray.Execute(10)


if __name__ == "__main__":
    unittest.main()
