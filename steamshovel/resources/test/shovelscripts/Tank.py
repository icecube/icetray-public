from icecube.shovelart import *
import math

class TankVertex( PyVariantVec3d ):

    orig_offset = [700, 0, 2000]

    def __init__( self, vtx ):
        PyVariantVec3d.__init__(self)
        self.vtx = tuple( [v + TankVertex.orig_offset[i] for i,v in enumerate(vtx) ] )

    def value( self, time ):
        norm = float(time % 1000) / 1000
        theta = norm * 2 * math.pi
        sin = math.sin( theta )
        cos = math.cos( theta )
        x = self.vtx[0] * cos - self.vtx[1] * sin
        y = self.vtx[0] * sin + self.vtx[1] * cos
        z = self.vtx[2]
        return vec3d( x, y, z )


class Tank( PyArtist ):

    numRequiredKeys = 0

    tankverts = [
      3,   0,  13,     5,   3,  15,     5,   3,  19,     3,   0,  21,
      -3,   0,  21,    -5,   3,  19,    -5,   3,  15,    -3,   0,  13,
      3,   0,  13,    -3,  60,  -3,     3,  60,  -3,     3,  60,   3,
      -3,  60,   3,    -3,  60,  -3,   -10, -45,  10,   -10, -13,  10,
      10, -13,  10,    10, -45,  10,    25, -50, -11,    25,  60, -20,
      10, -13,  10,   -10, -13,  10,   -25,  60, -20,   -25, -50, -11,
      -10, -45,  10,    10, -45,  10,    30, -53, -40,    35, -60, -10,
      35,  60, -20,    30,  37, -40,   -30,  37, -40,   -35,  60, -20,
      -35, -60, -10,   -30, -53, -40,    30, -53, -40,    30,  37, -40,
      5, -40,  10,     5, -40,  13,     3,   4,   3,     3,  60,   3,
      -3,   4,   3,    -3,  60,   3,    -3,  19,  -3,    -3,  60,  -3,
      3,  19,  -3,     3,  60,  -3,    35, -60, -10,   -35, -60, -10,
      35,  60, -20,   -35,  60, -20,   -30, -53, -40,   -30, 37, -40 ]


    tankverts = list(zip( *[iter(tankverts)] * 3 ))
 

    def create( self, frame, group ):
        vertices = VariantVec3dList()

        for (x,y,z) in self.tankverts:
            vertices.append( TankVertex((x,y,z)) )

        group.addDynamicLines( vertices )

