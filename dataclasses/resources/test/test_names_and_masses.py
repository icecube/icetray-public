#!/usr/bin/env python3

"""Test to see if the masses in I3Particle match the masses in the scikit-hep's particle package.

There is some ambiguity between our naming scheme and scikit's, so the tests do some string manipulation.
Our mass must agree with theirs within 0.7%.
see https://github.com/scikit-hep/particle
"""

import unittest

from icecube.dataclasses import I3Particle
from numpy.testing import assert_allclose
from particle import Particle


class TestParticleMasses(unittest.TestCase):
    def test_particle_masses(self):
        particle_types = I3Particle.ParticleType.values
        part = I3Particle()

        for pid in particle_types:
            # skip particles we know aren't valid PDG codes
            if pid in [0, 10000000] or abs(pid) > 2000000000:
                continue

            part.type = I3Particle.ParticleType(pid)
            pp = Particle.from_pdgid(pid)
            with self.subTest(type=part.type, particle=pp):
                pp_mass = pp.mass / 1e3 if pp.mass is not None else 0.0
                assert_allclose(part.mass, pp_mass, rtol=0.007)

                if pid > 1000000000:
                    ts = part.type_string.replace("H2", "D2").replace("H3", "T3")
                    self.assertEqual(ts, pp.name + "Nucleus")
                elif part.type_string == "PPlus":
                    self.assertEqual(pp.name, "p")
                elif part.type_string == "PMinus":
                    self.assertEqual(pp.name, "p~")
                elif part.type_string == "DsMinusBar":
                    self.assertEqual(pp.name, "D(s)-")
                else:
                    ts = part.type_string.lower().replace("bar", "~").replace("plus", "+").replace("minus", "-")
                    ts = ts.replace("neutron", "n").replace("zero", "0").replace("prime", "'").replace("star", "*")
                    ts = ts.replace("0_long", "l0").replace("0_short", "s0")
                    if not part.type_string.startswith("Sigmac2520"):
                        ts = ts.replace("-~", "~-").replace("0~", "~0").replace("+~", "~+")
                    pp_name = pp.name.lower().replace("(", "").replace(")", "").replace("/", "")
                    self.assertEqual(ts, pp_name)


if __name__ == "__main__":
    unittest.main()
