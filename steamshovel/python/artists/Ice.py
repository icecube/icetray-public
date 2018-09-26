from icecube.shovelart import PyArtist, PyQColor, RangeSetting
from icecube.dataclasses import I3Constants
from .util import parse_engineering
from random import Random


class Ice(PyArtist):

    """
    Shows the surface layer (ice) and the bottom layer (bedrock).
    """

    numRequiredKeys = 0

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            "Show ice", True,
            "Show bedrock", True,
            "Show dust", False,
            "3D dust", False,
            "Color ice", PyQColor.fromRgbF(0.1, 0.1, 1.0),
            "Color bedrock", PyQColor.fromRgbF(0.5, 0.4, 0.4),
            "Color dust", PyQColor.fromRgb(100, 100, 100, 50),
            "Plane width", "2200 m",
            "Line width", RangeSetting(0.1, 10., 1000, 1.0),
            "Dust density", RangeSetting(1.0, 4.0, 30, 1.5),
            "Dust scatter", RangeSetting(0.0, 1.0, 100, 0.2),
        ))

    def create( self, frame, group ):
        plane_width = parse_engineering(self.setting("Plane width"), "m")
        line_width = self.setting("Line width")

        if self.setting('Show ice'):
            icetop = group.addZPlane( I3Constants.zIceTop, plane_width, plane_width )
            icetop.setColor( self.setting("Color ice") )
            icetop.setLineWidth( line_width )
        if self.setting('Show bedrock'):
            bedrock = group.addZPlane( -I3Constants.OriginElev, plane_width, plane_width )
            bedrock.setColor( self.setting("Color bedrock") )
            bedrock.setLineWidth( line_width )
        if self.setting('Show dust'):
            dust_color = self.setting("Color dust")
            if self.setting('3D dust'):
                ran = Random(1)
                sphere_radius = 100
                f = self.setting("Dust density") # density adjustment
                n = int(f * plane_width / sphere_radius ) # no. of spheres per dimension
                # adjust alpha to density factor
                dust_color.alpha = int( dust_color.alpha / f ** 1.5)
                # fog looks better with randomness
                scatter = self.setting("Dust scatter")
                for ix in range(n):
                    x = (float(ix + 0.5) / n - 0.5) * plane_width
                    for iy in range(n):
                        y = (float(iy + 0.5) / n - 0.5) * plane_width
                        xr = x + plane_width / n * ran.gauss(0.0, scatter)
                        yr = y + plane_width / n * ran.gauss(0.0, scatter)
                        zr = -100 + 100 * ran.gauss(0.0, scatter)
                        s = group.addSphere( sphere_radius, (xr,yr,zr) )
                        s.setColor( dust_color )
                        s.setShader( 0 ) # to get flat spheres
            else:
                for z in (0, -200):  # top layer, bottom layer
                    dust = group.addZPlane( z, plane_width, plane_width )
                    dust.setColor( dust_color )
                    dust.setLineWidth( line_width )
    
