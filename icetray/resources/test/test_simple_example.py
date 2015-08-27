#!/usr/bin/env python

import os
import unittest
import subprocess

class TestSimpleExample(unittest.TestCase):

    def run_example(self, example_name):
        return_code = subprocess.call(
            ("../examples/" + example_name),
            cwd = os.path.dirname(__file__)
            )
        self.assertEqual(return_code, 0, "Return code should be 0.")

    def test_run(self):
        self.run_example('simple_example.py')

unittest.main()
