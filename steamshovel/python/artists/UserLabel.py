from icecube.shovelart import *

class UserLabel( PyArtist ):

    numRequiredKeys = 0

    def __init__(self):
        PyArtist.__init__(self)
        font = PyQFont()
        self.defineSettings((
        	'text', 'Text label',
        	'font', font,
        	'fontsize', font.pointSize
    	))

    def create( self, frame, output ):
        s = self.setting('text')
        f = self.setting('font')
        output.addTextOverlay( s, f )
