#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys

from icecube.icetray.i3logging import log_fatal
from icecube.dataclasses import I3Particle

total_test = 5
failed_test_counter = 0


# Test that particle and anti-particle have the same mass
particle_types = I3Particle.ParticleType.values
pdg_codes = particle_types.keys()
ptypes = particle_types.items()
try:
    for pdg, ptype in particle_types.items():
        if pdg == 0:
            # ParticleType.unknown has no mass implemented and is its own anti-particle
            continue
        if (pdg in pdg_codes) and (-pdg in pdg_codes):
            particle = I3Particle()
            particle.type = ptype
            antiparticle = I3Particle()
            antiparticle.type = particle_types[-pdg]
            assert particle.mass == antiparticle.mass, \
                "The masses for particle and anti-particle are different for " + \
                f"{particle.type_string}/{antiparticle.type_string}"

except AssertionError:
    failed_test_counter += 1

# Test that energy is equal to kinetic energy
particle = I3Particle()
particle.type = particle_types[211] # PiPlus
particle.energy = 100.
try:
    assert particle.energy == particle.kinetic_energy, \
        "I3Particle::GetEnergy() and I3Particle.GetKineticEnergy() return different values."
except AssertionError:
    failed_test_counter+= 1

# Test if conversion from total energy -> kinetic energy -> total energy works
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
total_energy = 100.
particle.total_energy = total_energy
kinetic_energy = particle.kinetic_energy
particle.kinetic_energy = kinetic_energy
try:
    assert particle.total_energy == total_energy, \
        "Conversion from total energy to kinetic energy and back returns different values."
except AssertionError:
    failed_test_counter += 1

# Test that you cannot set negative kinetic energy over total energy setting
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
try:
    particle.total_energy = 0.  # should throw a RuntimeError
    failed_test_counter += 1
except RuntimeError:
    pass

# Test that it is not possible to set mass of a particle
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
try:
    particle.mass = 100.  # should throw an Attribute Error
    failed_test_counter += 1
except AttributeError:
    pass

# Test that you cannot set total energy if mass is not implemented
particle = I3Particle()
particle.type = particle_types[-2000000041] # monopole
try:
    particle.total_energy = 100.
except AttributeError:
    # This should not happen when the c++ exception is handled correctly with the pybindings
    failed_test_counter += 1
except RuntimeError:
    pass
except Exception:
    failed_test_counter += 1

if failed_test_counter > 0:
    log_fatal(f"I3Particle masses: {failed_test_counter} of {total_test} test failed!")
else:
    print("I3Particle masses: all tests passed!")
    sys.exit(0)
