#!/usr/bin/env python3

import os
from string import hexdigits
from packaging import version
from icecube import icetray
import unittest


class TestGITStrings(unittest.TestCase):
    def test_git_strings(self):
        self.assertGreater(len(icetray.GIT_BRANCH), 0)
        self.assertEqual(len(icetray.GIT_REVISION), 40)
        self.assertTrue(all(c in set(hexdigits) for c in icetray.GIT_REVISION))
        self.assertTrue("icecube/icetray" in icetray.GIT_URL)
        self.assertTrue("github.com" in icetray.GIT_URL)
        self.assertGreaterEqual(
            version.parse(icetray.__version__.split("-")[0]), version.Version("1.8.2")
        )

    def test_matches_cmake_cache(self):
        vars = ("GIT_URL", "GIT_BRANCH", "GIT_REVISION")
        with open(os.environ["I3_BUILD"] + "/CMakeCache.txt", "r") as f:
            cache_vals = {
                var: line.strip().split("=")[-1]
                for line in f.readlines()
                for var in vars
                if line.startswith(var + ":")
            }
        print(cache_vals)
        for var in vars:
            self.assertEqual(cache_vals[var], getattr(icetray, var))
        self.assertEqual(icetray.__version__, icetray.PROJECT_VERSION)


if __name__ == "__main__":
    unittest.main()
