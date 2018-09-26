from .mplart import MPLArtist
from icecube.shovelart import ChoiceSetting, PyQColor, RangeSetting
from icecube import dataclasses


class AngleClock(MPLArtist):
    'Display a clock with two handles that show the elevation and azimuth of a particle.'

    numRequiredKeys = 1

    def isValidKey( self, frame, key_idx, key ):
         'Accept I3Particle or I3Direction.'
         tn = frame.type_name(key)
         return tn in ("I3Particle", "I3Direction")

    def __init__(self):
        MPLArtist.__init__(self)
        self.defineSettings((
            "show", ChoiceSetting("elevation & azimuth;elevation;azimuth", 0),
            "direction", ChoiceSetting(("destination", "origin"), 0),
            "elevation length", RangeSetting(0.0, 1.0, 10, 0.5),
            "elevation color", PyQColor.fromRgbF(0.9, 0.1, 0.1),
            "elevation width", 4,
            "azimuth length", RangeSetting(0.0, 1.0, 10, 0.9),
            "azimuth color", PyQColor.fromRgbF(0.1, 0.1, 0.9),
            "azimuth width", 3,
        ))

    def create_plot(self, frame, fig):
        from math import pi, degrees

        fobj = frame[self.keys()[0]]

        if (isinstance(fobj, dataclasses.I3Particle)):
            direc = fobj.dir
        elif (isinstance(fobj, dataclasses.I3Direction)):
            direc = fobj

        if self.setting("direction") == 1:
            direc = -direc

        ax = fig.add_subplot(111, polar=True)
        angles = (("elevation", "azimuth"), ("elevation",), ("azimuth",))
        for angle in angles[self.setting("show")]:
            l = self.setting(angle + " length")
            c = self.setting(angle + " color").rgbF()
            w = self.setting(angle + " width")
            a = ( pi / 2 - direc.theta
                  if angle == "elevation" else
                  direc.phi )
            ax.arrow( a, 0, 0, l, color=c, linewidth=w, head_width=0.05 * w, head_length=0.05)
        ax.get_yaxis().set_visible( False )
        ax.set_ylim( 0.0, 1.0 )
        ax.grid( True )
