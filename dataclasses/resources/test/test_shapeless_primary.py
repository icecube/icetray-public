#!/usr/bin/env python

# This test is slightly different than those in test_mctree_physics_library
# in that it uses I3MCTreeUtils to add the primary, which sets the shape
# to Primary on the fly.
# There are trees in production where this shape is not set and the methods
# that got the primary would fail when the shape wasn't set.  Currents methods
# don't rely on the shape being set to Primary.

import unittest

from I3Tray import I3Units
from icecube import dataclasses as dc

from icecube.dataclasses import get_most_energetic_primary

class TestShapelessParticle(unittest.TestCase):
    def test_get_most_energetic_primary(self):
        primary1 = dc.I3Particle()
        primary1.energy = 10 * I3Units.TeV
    
        primary2 = dc.I3Particle()
        primary2.energy = 1 * I3Units.TeV

        tree = dc.I3MCTree()
        tree.insert(primary1)
        tree.insert(primary2)

        mep = get_most_energetic_primary(tree)
        self.assertTrue(mep, "got no particles, but there are primaries.")
        self.assertEqual(mep.id, primary1.id, "got the wrong particle.")

unittest.main()
        
