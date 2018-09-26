from icecube import dataclasses
from icecube.shovelart import PyArtist, PyQColor, PyQFont, RangeSetting, \
                              vec3d, ConstantVec3d
from icecube.icetray import I3Units
from .util import parse_engineering

"""
DOMOrientation - A steamshovel artist to draw the orientation of a DOM
   This code is the result of a IceCube Bootcamp 2016 guided project
   initiated on Thursday, June 16th, 2016. The guide for the project
   was David Schultz, and the authors were Alex Burgman and Patrick Meade.

   Draws three arrows to indicate the orientation of the DOM
   The blue arrow indicates the direction the DOM is pointed
   The red arrow indicates the direction that Flasher #7 is pointed
   The green arrow is perpendicular to both the DOM and Flasher #7
   
   Based on code from CoordinateSystem.py, PhotonPaths.py, and Detector.cpp.
   Fun fact: Review of Detector.cpp resulted in Ticket #1749
             http://code.icecube.wisc.edu/projects/icecube/ticket/1749
"""
class DOMOrientation(PyArtist):

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            #"position", "0m, 0m, 0m",
            "Draw Default Orientation", False,
            "Draw DOM Direction", True,
            "Draw Flasher Direction", True,
            "Draw Orthogonal Direction", True,
            "Label Axes", False,
            "flasher length", "10m",
            "ortho length", "10m",
            "dom length", "10m",
            "line width", RangeSetting(0.1, 10., 1000, 10.0),
            "head angle", RangeSetting(0.0, 170, 100, 20.0),
            "head length", RangeSetting(0.0, 100.0, 100, 1.0),
            "opacity", RangeSetting(0.0, 1.0, 100, 1.0),
        ))

    requiredTypes = [ dataclasses.I3Geometry ]
    numRequiredKeys = 1

    """
        https://stackoverflow.com/a/33024979
    """
    def isclose( self, a, b, rel_tol=1e-09, abs_tol=0.0 ):
        return (abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol))
    
    def isDefaultDOMDirection(self, i3dir):
        return self.isclose(i3dir.x, 0.0, 1e-09, 1e-12) and self.isclose(i3dir.y, 0.0, 1e-09, 1e-12) and self.isclose(i3dir.z, -1.0, 1e-09, 1e-12)

    def isDefaultFlasherDirection(self, i3dir):
        return self.isclose(i3dir.x, 1.0, 1e-09, 1e-12) and self.isclose(i3dir.y, 0.0, 1e-09, 1e-12) and self.isclose(i3dir.z, 0.0, 1e-09, 1e-12)
    
    def create( self, frame, group ):
        # obtain the geometry of the detector
        geo = frame[list(self.keys())[0]]
        geo = frame["I3Geometry"]
        # obtain the geometry of the optical modules
        omgeo = geo.omgeo
        # for each optical module with geometry
        count = 0
        for omkey in omgeo.keys():
            # obtain the optical module
            dom = omgeo[omkey]
            # obtain the position and orientation of the optical module
            position = dom.position
            orientation = dom.orientation
            # draw the orientation arrows at the specified position
            self.createOrientation ( group, position, orientation, count, omkey )
            count = count + 1
        
    def createOrientation( self, group, position, orientation, count, omkey ):
        #spos = self.setting("position").strip("()").split(",")
        #pos = [parse_engineering(x, "m") * I3Units.meter for x in spos]
        #if len(pos) != 3:
        #    raise ValueError(str(pos) + " has wrong size")
        #pos = vec3d(pos[0], pos[1], pos[2])
        pos = vec3d(position.x, position.y, position.z)
        
        arrowDir = [orientation.up, orientation.right, orientation.dir]
        
        if(self.setting("Draw Default Orientation") == False):
            if(self.isDefaultFlasherDirection(arrowDir[0]) and self.isDefaultDOMDirection(arrowDir[2])):
                return

        """
        if(self.isclose(arrowDir[0].x, 1.0)):
            if(self.isclose(arrowDir[0].y, 0.0, 1e-09, 1e-12)):
                if(self.isclose(arrowDir[0].z, 0.0, 1e-09, 1e-12)):
                    if(count > 10000):
                        return
        print("i:%d  omkey:%s  pos:%f,%f,%f  up:%f,%f,%f" % (count, omkey, position.x, position.y, position.z, orientation.up.x, orientation.up.y, orientation.up.z))
        """
        drawArrow = [self.setting("Draw Flasher Direction"), self.setting("Draw Orthogonal Direction"), self.setting("Draw DOM Direction")]
        xlen = parse_engineering(self.setting("flasher length"), "m") * I3Units.meter
        ylen = parse_engineering(self.setting("ortho length"), "m") * I3Units.meter
        zlen = parse_engineering(self.setting("dom length"), "m") * I3Units.meter
        lw = self.setting("line width")
        angle = self.setting("head angle") * I3Units.degree
        length = self.setting("head length") * I3Units.m
        alpha = self.setting("opacity")

        for i in range(3):
            if(drawArrow[i]):
                delta = vec3d( xlen * arrowDir[i].x,
                               ylen * arrowDir[i].y,
                               zlen * arrowDir[i].z )
                self.createArrow(group, pos, delta, angle, length, i, alpha, lw)

    def createArrow( self, group, pos, delta, angle, length, i, alpha, lw ):
            zzz0 = pos
            zzz1 = pos + delta
            a = group.addArrow( ConstantVec3d(pos),
                                ConstantVec3d(pos + delta),
                                angle, length )
            a.setColor(( int(i == 0),
                         int(i == 1),
                         int(i == 2),
                         alpha ))
            a.setLineWidth( lw )
            if(self.setting("Label Axes")):
                tx = group.addText( "7OD"[i], pos + delta * 1.05 )
                font = PyQFont()
                font.pointSize = 8
                tx.setFont(font)
