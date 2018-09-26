#!/usr/bin/env python

import sys

from icecube import dataio, dataclasses
from os.path import expandvars

from icecube.icetray import I3Test
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
            # ParticleType.unkown has no mass implemented and is its own anti-particle
            continue
        if (pdg in pdg_codes) and (-pdg in pdg_codes):
            particle = I3Particle()
            particle.type = ptype
            antiparticle = I3Particle()
            antiparticle.type = particle_types[-pdg]
            I3Test.ENSURE_EQUAL(particle.mass, antiparticle.mass, 
                "The masses for particle and anti-particle are different for %s/%s." % (particle.type_string, antiparticle.type_string))
except:
    failed_test_counter += 1

# Test that energy is equal to kinetic energy
particle = I3Particle()
particle.type = particle_types[211] # PiPlus
particle.energy = 100.
try:
    I3Test.ENSURE_EQUAL(particle.energy, particle.kinetic_energy, 
        "I3Particle::GetEnergy() and I3Particle.GetKineticEnergy() return different values.")
except:
    failed_test_counter+= 1

# Test if conversion from total energy -> kinetic energy -> total energy works
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
total_energy = 100.
particle.total_energy = total_energy
kinetic_energy = particle.kinetic_energy
particle.kinetic_energy = kinetic_energy
try:
    I3Test.ENSURE_EQUAL(particle.total_energy, total_energy,
        "Conversion from total energy to kinetic energy and back returns different values.")
except:
    failed_test_counter += 1

# Test that you cannot set negative kinetic energy over total energy setting
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
try:
    particle.total_energy = 0.
    failed_test_counter += 1
except:
    pass

# Test that it is not possible to set mass of a particle
particle = I3Particle()
particle.type = particle_types[211]# PiPlus
try:
    particle.mass = 100.
    failed_test_counter += 1
except:
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
except:
    failed_test_counter += 1

print("I3Particle masses: %s of %s test failed!" % (failed_test_counter, total_test))
if failed_test_counter > 0:
    sys.exit(1)
else:
    sys.exit(0)
