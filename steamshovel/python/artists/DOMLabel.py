from icecube.shovelart import *
from icecube import dataclasses
from matplotlib.colors import colorConverter as mplcol

class DOMLabel( PyArtist ):

    requiredTypes = [dataclasses.I3Geometry]

    def __init__(self):
        PyArtist.__init__(self)
        font = PyQFont()
        self.defineSettings((
            'selection', OMKeySet(),
            'font', font,
            'fontsize', font.pointSize
        ))

    def create( self, frame, output ):
        omlist = self.setting( 'selection' )
        if( len(omlist) > 0 ):
            geom = frame[ self.keys()[0] ]
            font = self.setting( 'font' )
            font.pointSize = self.setting( 'fontsize' )
            for omkey in omlist:
                text = output.addTextOverlay( str(omkey), font )
                line = output.addOverlayLine( text, geom.omgeo[omkey].position )
                line.setLineWidth( 2 )
                line.setColor( PyQColor.fromRgbF(1.,0.,0.) )
