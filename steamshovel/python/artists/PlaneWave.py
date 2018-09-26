from icecube.shovelart import *
from icecube.dataclasses import I3Particle

class PlaneWave( PyArtist ):
    '''Create a pseudo-plane object perpendicular to a track'''

    requiredTypes = [ I3Particle ]

    def __init__( self ):
        PyArtist.__init__(self)
        self.defineSettings( { 'scale':RangeSetting(0.1,10.0,330,1.0),
                               'color':PyQColor(200,200,200) } )

    def create( self, frame, output ):
        particle = frame[ self.keys()[0] ]
        if( particle.is_track ):
            scale = self.setting('scale')
            color = self.setting('color')
            vertex = ParticlePoint( output, particle, TimePoint.CURRENT )
            # approximate a disc by making the axis vector small relative to the base radius.
            cyl = output.addCylinder( vec3d(0,0,0), vec3d(particle.dir)*.1*scale, 200.*scale )
            cyl.setLocation( vertex )
            cyl.setColor( color )

