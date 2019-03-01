===========
I3Position
===========
The I3Position class stores a position in cartesian coordinates. It can be initialized in either cartesian, cylindrical, or spherical coordinates.  
::

 Cartesian: x, y, z
 Spherical = r, θ, φ 
 Cylindrical = ρ, φ, z

The class allows for rotation of coordinates around x, y, and z axis and also transforms between the different coordinate parametrizations. Additionally, it computes the magnitude of the position vector and the cross product of two position vectors.

Examples
========
For initialization and transformation between coordinate systems:
::

 # initialization in cartesian coordinates. 
 # dataclasses.I3Position(x, y, z, RefFrame)
 # Default RefFrame (reference frame) is the cartesian: dataclasses.I3Position.car = dataclasses.RefFrame.car 
 >>> position = dataclasses.I3Position(1, 2, 3)
 # cartesion coordinates
 >>> position.x
 1.0
 >>> position.y
 2.0
 >>> position.z
 3.0

 # spherical coordinates
 >>> position.theta
 0.6405223126794245
 >>> position.phi
 1.1071487177940904
 >>> position.r
 3.7416573867739413

 # cylindrical coordinates
 >>> position.rho
 2.2360679774997894
 >>> position.phi
 1.1071487177940904
 >>> position.z
 3.0

 # initialize a position in a cylindrical coordinates (cylindrical reference frame):
 >>> position2 = dataclasses.I3Position(4, 5, 6, dataclasses.I3Position.cyl)
 >>> position2 # the position itself is still by standard given in cartesian coordinates
 I3Position(1.13465,-3.8357,6)
 >>> position2.x
 1.134648741852905
 >>> position2.rho
 4.0
 >>> position2.phi
 5.0
 >>> position2.z
 6.0

 # initialize in spherical coordinates:
 >>> position2 = dataclasses.I3Position(4, 5, 6, dataclasses.I3Position.sph)
 >>> position2
 I3Position(-3.68292,1.07175,1.13465)
 >>> position2.r
 4.0
 >>> position2.theta
 5.0
 >>> position2.phi
 6.0

Operations on I3Position:
::

 # calculate magnitude = sqrt(x^2 + y^2 + z^2)
 >>> position
 I3Position(1,2,3)
 >>> position.magnitude
 3.7416573867739413
 # calculate x^2 + y^2 + z^2
 >>> position.mag2
 14.0

 # cross product between two positions vectors: position and position2
 >>> position2 = dataclasses.I3Position(4, 5, 6)
 >>> position.cross(position2)
 I3Position(-3,6,-3)

Rotation of position vector with x, y, or z as rotation axis around an angle a given in radians:
:: 

 # rotation of position vector around an angle a in radians
 >>> position
 I3Position(1,2,3)
 >>> a = 3.1415 # rotate 180°
 >>> position.rotate_x(a) # rotate 180° around x axis
 >>> position
 I3Position(1,-2.00028,-2.99981)
 >>> position.rotate_x(a)
 >>> position
 I3Position(1,2.00056,2.99963)
 >>> position.rotate_x(-2 * a) # rotate -360° around x axis
 >>> position
 I3Position(1,2,3)
 >>> position.rotate_y(a) # rotate 180° around y axis
 >>> position
 I3Position(-0.999722,2,-3.00009)
 >>> position.rotate_z(a) # rotate 180° around z axis
 >>> position
 I3Position(0.999537,-2.00009,-3.00009)


