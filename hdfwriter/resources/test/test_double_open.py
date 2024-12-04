#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest

import h5py
from icecube import icetray
from icecube.hdfwriter import I3SimHDFWriter, OutputFileInUseError
from icecube.icetray import I3Tray

FILE_NAME = "hdfwriter_test_double_open.hdf5"


def run_tray(count=1, **hdfwriter_kwargs):
    tray = I3Tray()

    tray.Add("I3InfiniteSource")

    tray.Add(
        lambda frame: frame.Put("int", icetray.I3Int(1)), Streams=[icetray.I3Frame.DAQ]
    )

    for _ in range(count):
        tray.Add(I3SimHDFWriter, Output=FILE_NAME, **hdfwriter_kwargs)

    try:
        tray.Execute(1)
    finally:
        tray.Finish()


class TestDoubleOpen(unittest.TestCase):
    def tearDown(self):
        os.unlink(FILE_NAME)

    def test_open_in_separate_trays(self):
        """
        Output filename can be reused in separate trays
        """
        self.assertFalse(os.path.exists(FILE_NAME))
        run_tray(Keys=["int"])
        self.assertTrue(os.path.exists(FILE_NAME))
        with h5py.File(FILE_NAME, "r") as f:
            self.assertIn("int", f)
        run_tray(Keys=["int"])
        with h5py.File(FILE_NAME, "r") as f:
            self.assertIn("int", f)

    def test_open_in_single_tray(self):
        """
        Output filename cannot be reused in the same tray
        """
        with self.assertRaises(OutputFileInUseError):
            run_tray(2, Keys=["int"])


if __name__ == "__main__":
    unittest.main()
