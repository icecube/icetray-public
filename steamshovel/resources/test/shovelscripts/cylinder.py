from icecube.shovelart import vec3d, ConstantVec3d, Vec3dList, PyQColor, PyArtist, ConstantFloat
from math import sqrt

class CylinderArtist( PyArtist ):

    numRequiredKeys = 0

    def create( self, frame, output ):

        # Some cylinders
        output.addCylinder( vec3d(0,0,0), vec3d( 0, 0, 100), 5, 25 )
        output.addCylinder( vec3d(0,0,100), vec3d( 100,100, 0), 25, 0 )
        output.addCylinder( vec3d(100,100,100), vec3d( -100,-100, -100), 0, 25 )

        # Some green, narrow cylinders
        for v in [ vec3d(*t) for t in ((0,0,-100), (100,0,0), (-100,0,0), (0,100,0), (0,-100,0)) ]:
            c = output.addCylinder( vec3d(0,0,0 ), v, 5 )
            c.setColor( PyQColor(128,255,128) )

        # Some lines along the first set of cylinder's axes
        points = Vec3dList()
        points.append( vec3d(0,0,0) )
        points.append( vec3d(0,0,100) )
        points.append( vec3d(100,100,100) )
        points.append( points[0] )
        output.addStaticLines( points )


        # Some spheres
        h1 = output.addSphere( 200, vec3d(400, 0, 500) )
        h1.setColor( (.9, .9, .9) )

        h2 = output.addSphere( 25, vec3d( 400, 220, 500 ) )
        h2.setColor( (.1,.1,.1) )

        h3 = output.addSphere( 25, vec3d( 400, -220, 500 ) )
        h3.setColor( (.1,.1,.1) )

        # Add cylinder using post-creation adjustments
        h4 = output.addCylinder( vec3d(0,0,0), vec3d(0,0,0), 0, 0 )
        h4.setLocation( vec3d(400,0,500) )
        h4.setAxis( ConstantVec3d((vec3d( -500, 0, 0 ))))
        h4.setBaseRadius( 90 )
        h4.setTopRadius( ConstantFloat(1) )
        h4.setColor( (.9, .9, .1) )

# Register and create artist, set camera, save output
window.gl.scenario.registerArtist(CylinderArtist)
window.gl.scenario.add("CylinderArtist")

window.gl.perspectiveView = False
window.gl.cameraPivot = ( 200, 0, 250 )
window.gl.cameraOrientation = (0.0, 1.0, 0.0)
window.gl.cameraLoc = ( 0, 500, -500 )

window.gl.screenshot( 200, 200, "cylinder.png" )

