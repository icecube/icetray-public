#!/usr/bin/env python

# The I3FrameObject base was missing in the python bindings.
# Now the I3ParticleVect inherits from I3FrameObject in python.
# just as it does in C++, so you can create them and add them
# to frames...just like in C++.

from icecube import icetray
from icecube import dataclasses

particle_vect = dataclasses.I3VectorI3Particle()

p1 = dataclasses.I3Particle()
particle_vect.append(p1)
p2 = dataclasses.I3Particle()
particle_vect.append(p2)
p3 = dataclasses.I3Particle()
particle_vect.append(p3)

frame = icetray.I3Frame()
frame['pv'] = particle_vect

# test out all the vector operations
if particle_vect[0] != p1:
    raise Exception('index particle not equal')

particle_vect2 = dataclasses.I3VectorI3Particle()
particle_vect2.append(p1)
particle_vect2.append(p2)
if particle_vect[:2] != particle_vect2:
    raise Exception('slice not equal')
del particle_vect[1]

particle_vect[-1] = p1
particle_vect[:1] = p2
particle_vect[:1] = particle_vect2
particle_vect[2:1] = p2
particle_vect[2:1] = particle_vect2
del particle_vect[:-2]

particle_vect3 = dataclasses.I3VectorI3Particle()
particle_vect3.append(dataclasses.I3Particle())

particle_vect.extend(particle_vect3)

if len(particle_vect[3:2]) > 0:
    raise Exception('inverted slice has contents')

try:
    particle_vect.append("blah")
except:
    pass
else:
    raise Exception('string append did not raise exception')
