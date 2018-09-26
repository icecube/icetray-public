from icecube import dataclasses
from icecube.shovelart import PyArtist, RangeSetting, PyQColor, vec3d, LinterpFunctionVec3d
from .util import particle_pos


class Position(PyArtist):
    """
    Draws a bubble marker for an I3Position or I3Particle,
    and for collections of those things.
    """
    numRequiredKeys = 1

    def isValidKey(self, frame, key_idx, key ):
        tname = frame.type_name(key)
        return tname in ("I3Position", "I3Particle",
                         "I3Vector<I3Position>",
                         "I3Vector<I3Particle>")

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            "size", RangeSetting(2.0, 30.0, 100, 10.0),
            "color", PyQColor.fromRgbF(0, 0, 1),
        ))

    def create(self, frame, output):
        size = self.setting('size')
        color = self.setting('color')

        fobj = frame[self.keys()[0]]

        # make sure we are iterating over a collection
        if (isinstance(fobj, dataclasses.I3Position) or
            isinstance(fobj, dataclasses.I3Particle)):
            fobj = [fobj]

        for obj in fobj:
            if isinstance(obj, dataclasses.I3Particle):
                particle = obj
                s = output.addSphere( size, particle.pos )
                posfunc = LinterpFunctionVec3d(particle_pos(particle, 0), 0)
                posfunc.add(particle_pos(particle, 40000), 40000)
                s.setLocation(posfunc)

            else:
                position = obj
                s = output.addSphere(size, position)

            s.setColor( color );
