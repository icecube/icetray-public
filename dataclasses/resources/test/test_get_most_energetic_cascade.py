#!/usr/bin/env python

from random import uniform
from random import random

from I3Tray import I3Units
from icecube import dataclasses as dc

from icecube.icetray import I3Test

from icecube.dataclasses import get_most_energetic_cascade
from icecube.dataclasses import get_most_energetic_inice_cascade

energies = list()
tree = dc.I3MCTree()
p0 = dc.I3Particle()
p0.energy = 100 * I3Units.GeV
p0.type = dc.I3Particle.PPlus
p0.location_type = dc.I3Particle.InIce
tree.add_primary(p0)
p1 = dc.I3Particle()
p1.energy = 80 * I3Units.GeV
p1.type = dc.I3Particle.EMinus
p1.location_type = dc.I3Particle.InIce
tree.append_child(p0,p1)
p2 = dc.I3Particle()
p2.energy = 20 * I3Units.GeV
p2.type = dc.I3Particle.EMinus
p2.location_type = dc.I3Particle.InIce
tree.append_child(p0,p2)
p3 = dc.I3Particle()
# set to NaN
#p3.energy = 10 * I3Units.GeV
p3.type = dc.I3Particle.Brems
p3.location_type = dc.I3Particle.InIce
tree.append_child(p1,p3)



 
print("most_energetic_cascade energy = %f" % get_most_energetic_cascade(tree).energy)
print("most_energetic_inice_cascade energy = %f" % get_most_energetic_inice_cascade(tree).energy)
    
I3Test.ENSURE(get_most_energetic_cascade(tree).energy == 80 * I3Units.GeV, "get_most_energetic_cascade failed")
I3Test.ENSURE(get_most_energetic_inice_cascade(tree).energy == 80 * I3Units.GeV, "get_most_energetic_inice_cascade failed")


