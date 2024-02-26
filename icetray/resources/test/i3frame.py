# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest

from icecube.icetray import I3Frame

class I3FrameTest(unittest.TestCase):

    def testPutNone(self):
        """Putting None in the frame raises TypeError"""
        frame = I3Frame()
        self.assertRaises(TypeError, frame.Put, "foo", None)

if __name__ == "__main__":
    unittest.main()
