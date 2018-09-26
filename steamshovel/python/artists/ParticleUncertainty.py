from icecube.shovelart import *
from icecube import paraboloid

import math

from .util import particle_pos

def axis2vertex(particle, time):
    return vec3d(-particle.dir*particle.speed*(time-particle.time))

def dist(particle, time):
    return math.sqrt((particle.dir*particle.speed*(time-particle.time)).mag2)

def bottom(particle, time, angle):
    return dist(particle, time) * math.tan(angle)

class ParticleUncertainty( PyArtist ):

    requiredTypes = [ paraboloid.I3ParaboloidFitParams ]

    def __init__( self ):
        PyArtist.__init__(self)

        self.defineSettings(dict(color=PyQColor.fromRgbF(0, 0, 1)))

    def create( self, frame, output ):
        params = frame[self.keys()[0]]
        particle = frame[self.keys()[0][:-len("FitParams")]]
        color = self.setting("color")

        # opening angle
        psi = math.sqrt((params.pbfErr1**2 + params.pbfErr2**2)/2.)

        nulltime = particle.time

        ### backward cylinder
        bcyl = output.addCylinder(particle_pos(particle, 0.),
                                  0. * vec3d(particle.dir),
                                  bottom(particle, 0., psi))

        # increasing height
        baxisFunc = LinterpFunctionVec3d(0. * vec3d(particle.dir), 0)
        baxisFunc.add(axis2vertex(particle, 0.), nulltime)
        baxisFunc.add(axis2vertex(particle, 0.), 40000)

        # decreasing radius
        btopFunc = LinterpFunctionFloat(bottom(particle, 0, psi), 0)
        btopFunc.add(0., nulltime)
        btopFunc.add(0., 40000)

        bcyl.setColor(color)
        bcyl.setAxis(baxisFunc)
        bcyl.setTopRadius(btopFunc)

        ### forward Cylinder
        fcyl = output.addCylinder(particle_pos(particle, 0.),
                                  0. * vec3d(particle.dir),
                                  bottom(particle, 0., psi))

        # moving vertex
        fvertexFunc = LinterpFunctionVec3d(vec3d(particle.pos), 0)
        fvertexFunc.add(vec3d(particle.pos), nulltime)
        fvertexFunc.add(particle_pos(particle, 40000.), 40000)

        # increasing height
        faxisFunc = LinterpFunctionVec3d(0. * vec3d(particle.dir), 0)
        faxisFunc.add(0. * vec3d(particle.dir), nulltime)
        faxisFunc.add(axis2vertex(particle, 40000.), 40000)

        # increasing radius
        fbottomFunc = LinterpFunctionFloat(0., 0)
        fbottomFunc.add(0., nulltime)
        fbottomFunc.add(bottom(particle, 40000., psi), 40000)

        fcyl.setColor(color)
        fcyl.setLocation(fvertexFunc)
        fcyl.setAxis(faxisFunc)
        fcyl.setBaseRadius(fbottomFunc)


