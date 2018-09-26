from icecube.shovelart import PyArtist, PyQColor, PyQFont, RangeSetting, \
                              vec3d, ConstantVec3d
from icecube.icetray import I3Units
from .util import parse_engineering

class CoordinateSystem(PyArtist):

    """
    Draws three arrows to indicate the x,y,z axis of the coordinate system.
    """

    numRequiredKeys = 0

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            "position", "0m, 0m, 0m",
            "x length", "1 km",
            "y length", "1 km",
            "z length", "1 km",
            "line width", RangeSetting(0.1, 10., 1000, 1.0),
            "head angle", RangeSetting(0.0, 170, 100, 20.0),
            "head length", RangeSetting(0.0, 100.0, 100, 30.0),
            "opacity", RangeSetting(0.0, 1.0, 100, 1.0),
        ))

    def create( self, frame, group ):
        spos = self.setting("position").strip("()").split(",")
        pos = [parse_engineering(x, "m") * I3Units.meter for x in spos]
        if len(pos) != 3:
            raise ValueError(str(pos) + " has wrong size")
        pos = vec3d(pos[0], pos[1], pos[2])

        xlen = parse_engineering(self.setting("x length"), "m") * I3Units.meter
        ylen = parse_engineering(self.setting("y length"), "m") * I3Units.meter
        zlen = parse_engineering(self.setting("z length"), "m") * I3Units.meter
        lw = self.setting("line width")
        angle = self.setting("head angle") * I3Units.degree
        length = self.setting("head length") * I3Units.m
        alpha = self.setting("opacity")

        for i in range(3):
            delta = vec3d( xlen * int(i == 0),
                           ylen * int(i == 1),
                           zlen * int(i == 2) )
            a = group.addArrow( ConstantVec3d(pos),
                                ConstantVec3d(pos + delta),
                                angle, length )
            a.setColor(( int(i == 0),
                         int(i == 1),
                         int(i == 2),
                         alpha ))
            a.setLineWidth( lw )
            tx = group.addText( "XYZ"[i], pos + delta * 1.05 )
            font = PyQFont()
            font.pointSize = 40
            tx.setFont(font)
