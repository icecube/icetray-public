#!/usr/bin/env python3

import os
import unittest

from icecube import icetray, dataclasses, dataio
from icecube.hdfwriter import I3SimHDFWriter
from I3Tray import I3Tray

FILE_NAME = "hdfwriter_test_slash_in_key_name.hdf5"

def run_tray(**hdfwriter_kwargs):
    tray = I3Tray()

    tray.context["I3FileStager"] = dataio.get_stagers()
    tray.Add("I3InfiniteSource")

    tray.Add(lambda frame: frame.Put("Bad/Hombre", icetray.I3Int(1)), Streams=[icetray.I3Frame.DAQ])

    tray.Add(I3SimHDFWriter,
        Output=FILE_NAME,
        **hdfwriter_kwargs
    )

    try:
        tray.Execute(1)
    finally:
        tray.Finish()

class TestStringMethods(unittest.TestCase):

    def tearDown(self):
        os.unlink(FILE_NAME)
    
    def test_key_with_slash(self):
        """frame keys that are not valid HDF5 dataset names raise ValueError"""
        with self.assertRaises(ValueError):
            run_tray(Keys=["Bad/Hombre"]), 
    
    def test_type_with_slash(self):
        with self.assertRaises(ValueError):
            run_tray(Types=[icetray.I3Int])

    def test_key_with_slash_and_valid_name(self):
        run_tray(Keys=[dict(key="Bad/Hombre", name='Bad_Hombre')])

if __name__ == '__main__':
    unittest.main()
