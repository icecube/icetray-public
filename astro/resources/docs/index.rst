.. @copyright (C) 2015 The IceCube Collaboration
.. 
.. $Id$
..
.. @author Kevin Meagher
.. @date August 2015
.. $Rev$
.. $LastChangedBy$
.. $LastChangedDate$

.. _astro:

astro
~~~~~

This Package provides functions for converting IceCube data to astronomical coordinates
and back. 

Maintainer: Kevin Meahger <kmeagher@ubl.ac.be>

Overview
========

In a typical IceCube analysis, only a few Astronomical coordinate transformations are needed. 
Structs for storing equatorial (J2000) and galactic coordinates are provided and functions for
converting between coordinate systems, :doc:`I3Direction </projects/dataclasses/i3direction>` from dataclasses is used for storing local
coordinates. Functions for calculating the direction of the Sun and moon are also provided.
The following diagram gives an overview of the functions provided by this project:

.. image:: I3Astro.svg


Rationale
=========

Offline Software had contained a *coordinate-service* package for
a long time, however it has problems which need to be addressed: The code follows an
overly circuitous and complicated path causing maintenance issues, it implements functions to convert to apparent
coordinates which are completely unneeded and causes confusion, and the functions are
very poorly named. Astro address these problems by providing a new clean implementation
with clear easy to understand function names and only implementing the function names
which are used.

Background
==========

Astronomical objects are most commonly located in the celestial sphere by a coordinate
system referred to as the equatorial coordinate system. This is the system in which the
universe is 'at rest', aside from solar system objects, and stars near the solar system,
astronomical objects do not move in equatorial coordinates. 
That is the celestial sphere which shares  the equator and poles with the Earth. The coordinates in
this system are Right Ascension (commonly abbreviated  *RA* or *α*) and Declination
(commonly abbreviated *Dec* or *δ*). In addition, a galactic coordinate system
is defined in which is centered a the the galaxy lies along the equator and is centered at
longitude =0°. This coordinate system uses galactic longitude (abbreviated *l*)
and galactic latitude (abbreviated *b*) as coordinates. An introduction to positional astronomy can be found `here <http://star-www.st-and.ac.uk/~fv/webnotes/index.html>`_.

The `IceCube Coordinate System <https://wiki.icecube.wisc.edu/index.php/Coordinate_system>`_ is slightly  different from the standard local coordinates used astronomy. 
Zenith angle is defined in the standard way, but azimuth angle is defined differently.
The standard definition of azimuth is North(0°) => East(90°) => South(180°)
=> West(270°). Since IceCube is at the pole it uses grid coordinates but the definition of
azimuth is still different, it is Grid East(0°) => Grid North(90°) => Grid West(180°)
=> Grid South(0°). Since an IceCube azimuth is defined as 90 - azimuth, it might be more
appropriate to call it *co-azimuth* but no one does that.

Unfortunately, the equatorial coordinate system can be ambiguous, although the direction of the
Earth's poles and equator is stable on the short term. The position of the poles slowly moves
due to the torque of the sun and moon on the Earth's equatorial bulge. To compensate for this
RA and dec positions are accompanied by an epoch, since 1984 most astronomical catalogs have
used the J2000 epoch, previous epoch were B1900 and B1950 (The J and B stand for Julian and
Besselian respectively which are different ways of defining epochs.) Astro uses J2000 exclusively
if another epoch becomes common, new function names can be added. Some times observations are
given in coordinates of apparent or current epoch, these coordinates should be considered an
intermediate calculation and no support is provided for them.

Examples
=========

Pybindings for Core Functions
-----------------------------

The following is an example of using astro in python:

.. code-block:: python
		
  from icecube.icetray import I3Units
  from icecube import dataclasses,astro

  #set time to 2015-3-21 11:11:37
  time = dataclasses.I3Time()
  time.set_utc_cal_date(2015,3,21,11,11,37,0)

  #calculate the direction of the crab pulsar
  crab = astro.I3Equatorial(83.63308*I3Units.degree,
                            22.01450*I3Units.degree)
  crab_dir = astro.I3GetDirectionFromEquatorial(crab,time)
  print (crab_dir.zenith/I3Units.degree,
         crab_dir.azimuth/I3Units.degree)

  #calculate the location of the crab pulsar in galactic coordinates
  crab_gal = astro.I3GetGalacticFromEquatorial(crab)
  print (crab_gal.l/I3Units.degree,
         crab_gal.b/I3Units.degree)

  #calculate the position of the moon
  moon_direction = astro.I3GetMoonDirection(time)
  print (moon_direction.zenith /I3Units.degree,
         moon_direction.azimuth/I3Units.degree)

  #calculate the location of the moon in ra, dec
  moon_equatorial = astro.I3GetEquatorialFromDirection(moon_direction,time)
  print (moon_equatorial.ra /I3Units.degree,
         moon_equatorial.dec/I3Units.degree)


Table Functions
---------------

These table functions are provided to make using ``astro`` with data written by :doc:`../tableio/index` with ``I3HDFWriter`` as easy as possible. Tables read with ``pytables``, ``h5py`` and ``pandas`` work equally well.

.. code-block:: python

    # using pytables
    f = tables.openFile('foo.hdf5')
    ra,dec= tables_to_equa(f.root.Particle,
                           f.root.I3EventHeader)

    #using h5py
    f= h5py.File('foo.hdf5')
    ra,dec = tables_to_equa(f["Particle"],
                            f["I3EventHeader"])

    #using pandas
    p = pandas.read_hdf('foo.hdf5','Particle')
    h = pandas.read_hdf('foo.hdf5','I3EventHeader')
    ra,dec = tables_to_equa(p,h)


Vector Functions
----------------

For your convenience, python numpy vector functions are also provided 

.. code-block:: python

   import tables
   import numpy as np
   from icecube import astro

   with tables.openFile('tableio.h5') as f:
   
     mjd = (f.root.I3EventHeader.cols.time_start_mjd_day[:]+
            f.root.I3EventHeader.cols.time_start_mjd_sec[:]/86400.)
     zenith  = f.root.OnlineL2_BestFit.cols.zenith[:]
     azimuth = f.root.OnlineL2_BestFit.cols.azimuth[:]

     ra,dec = astro.dir_to_equa(zenith,azimuth,mjd)

     print np.degrees(ra)
     print np.degrees(dec)

     #calculate distance to the Crab Nebula
     print astro.angular_distance(ra,dec,
				  np.radians(83.63308)
				  np.radians(22.01450)
				  )


Tablio Bookers
--------------
.. code-block:: python

  tray.Add(tableio.I3TableWriter,
           output="foo.hdf5",
           SubEventStreams=['in_ice'],
           keys = {'Particle': [ dataclasses.converters.I3ParticleConverter(),
                                 astro.converters.I3AstroConverter(),
				 astro.converters.I3GalacticConverter(),
				 astro.converters.I3SuperGalacticConverter(),
				 ],
		   'I3EventHeader' : [ dataclasses.converters.I3EventHeaderConverter(),
		                       astro.converters.I3SunAndMoonConverter()]
                  }
	 
Implementation
==============

I3Astro provides 3 container classes:

* :py:class:`icecube.astro.I3Equatorial` for storing J2000 coordinates      
    :``ra``: Right ascension in radians
    :``dec``: Declination in radians	    
* :py:class:`icecube.astro.I3Galactic` for storing IAU1958 galactic coordinates     
    :``l``: Galactic Longitude in radians
    :``b``: Galactic Latitude in radians	  
* :py:class:`icecube.astro.I3SuperGalactic` for storing de Vaucouleurs supergalactic coordinates
    :``l``: Galactic Longitude in radians
    :``b``: Galactic Latitude in radians

I3Astro provides 10 functions:

* :py:func:`icecube.astro.I3GetMoonDirection` Gets the direction of the Moon in local IceCube coordinates at a certian time
* :py:func:`icecube.astro.I3GetSunDirection` Gets the direction of the Sun in local IceCube coordinates at a certian time
* :py:func:`icecube.astro.I3GetDirectionFromEquatorial` Convert a an position in J2000 equatorial coordinates to local IceCube coordinates at a given time
* :py:func:`icecube.astro.I3GetEquatorialFromDirection` Convert an IceCube direction to J2000 equatorial coordinates system at a given time
* :py:func:`icecube.astro.I3GetGalacticFromEquatorial` Convert from Equatorial (J2000) to Galactic (IAU 1958) coordinate system
* :py:func:`icecube.astro.I3GetEquatorialFromGalactic` Convert from Galactic (IAU 1958) to Equatorial (J2000) coordinate system
* :py:func:`icecube.astro.I3GetSuperGalacticFromGalactic` Convert from Galactic (IAU 1958) to SuperGalactic (de Vaucouleurs) coordinate system
* :py:func:`icecube.astro.I3GetGalacticFromSuperGalactic` Convert from SuperGalactic (de Vaucouleurs) to Galactic (IAU 1958) coordinate system
* :py:func:`icecube.astro.I3GetSuperGalacticFromEquatorial` Convert from Equatorial (J2000) to SuperGalactic (de Vaucouleurs) coordinate system
* :py:func:`icecube.astro.I3GetEquatorialFromSuperGalactic` Convert from SuperGalactic (de Vaucouleurs) to Equatorial (J2000) coordinate system

I3Astro provides 4 :doc:`../tableio/index` converters:

* :js:data:`I3AstroConverter` books equatorial coordinates 
* :js:data:`I3GalacticConverter` books galactic coordinates 
* :js:data:`I3SuperGalacticConverter` books supergalactic coordinates
* :js:data:`I3SunAndMoonConverter` books the location of the Sun and moon for a timestamp provided by :cpp:class:`I3EventHeader`

Accuracy
========

Positional astronomy calculations can be extremely accurate, standard software such as PAL
can achieve milliarcsecond precision when all of the corrections are applied correctly.
However for IceCube data, muon/cascade reconstruction error is the dominant
source of angular uncertainty, this is typically O(1°) for muon neutrinos and higher for
cascades.
None of the corrections typically applied by these packages (save nutation)
are necessary to achieve the necessary level of accuracy for an IceCube analysis. 
However, discussion of positional astronomy often lead to arguments about what corrections
are appropriate to apply.
The following table and discussion is intended to provide context to any future
discussions about which corrections are appropriate to apply.

+------------------------+------------+-------------+---------------------------+
| Effect                 | Magnitude  | included in | Remark                    |
|                        |            | calculation |                           |
+========================+============+=============+===========================+
| proper motion and      | source     |             | only affects stars close  |
| parallax               | dependent  | No          | to the solar system       |
+------------------------+------------+-------------+---------------------------+
| precession             | 50"/yr     | Yes         | relative to the year 2000 |
+------------------------+------------+-------------+---------------------------+
| nutation               | < 11"      | Yes         |                           |
+------------------------+------------+-------------+---------------------------+ 
| annual aberration      | < 20.5"    | Yes         |                           |
+------------------------+------------+-------------+---------------------------+
| light deflection       | < 1.74"    | Yes         | only near the sun limb    |
+------------------------+------------+-------------+---------------------------+
| Sun positional         |            |             |                           |
| uncertainty            |  5"        |             |                           |
+------------------------+------------+-------------+---------------------------+
| Moon positional        |            |             |                           |
| uncertainty            | 30"        |             |                           |
+------------------------+------------+-------------+---------------------------+
| diurnal aberration     | < 0.3"     | Yes         |                           |
+------------------------+------------+-------------+---------------------------+
| refraction             | < 0.5°     | No          | Cosmic Rays and Neutrinos |
|                        |            |             | are not refracted         |
+------------------------+------------+-------------+---------------------------+
| polar motion           | < 0.3"     | No          |                           |
+------------------------+------------+-------------+---------------------------+
| UT1-UTC offset         | < 13.5"    | No          |                           |
+------------------------+------------+-------------+---------------------------+
| IceCube Not at Exact   |            |             |                           |
| South Pole             | 32"        | Yes         |                           |
+------------------------+------------+-------------+---------------------------+
| IceCube Size           | < 16"      | No          |                           |
+------------------------+------------+-------------+---------------------------+
| Glacial flow           | 0.3"/yr    | No          | relative to 2011          |
+------------------------+------------+-------------+---------------------------+


Nutation, annual aberration, and light deflection are included in the same function as
precession so it is easier to include them than to exclude them. (Also if they are included the
accuracy of the package can be compared with other high accuracy astronomy software.)

Parallax and proper motion only effect stars within O(10 parsecs) of the solar system.
This can be ignored as there are no IceCube analyses which study objects at this distance.
To calculate polar motion and UT1-UTC it is necessary to downloading data files from the
International Earth Rotation and Reference System Service (IERS) which are updated on a monthly
basis.
Due to the hassle of downloading and making sure these files are up to date, these correction
are omitted.

The quoted Sun and Moon uncertainties are the uncertainties of the algorithm used to predict the
position of the moon and sun by PAL.

Astro uses geographic position 89°59'39.84" South, 62°36'29.16" West (-89.9944°N,-62.6081°E)
as the location of IceCube observations.
This value is taken from the 2011 position of the South Pole in IceCube coordinates system on the
`wiki <https://wiki.icecube.wisc.edu/index.php/Coordinate_system>`_.
IceCube reconstructions calculate zenith and azimuth angles relative to a position
near the center of gravity of the hits, which can be up to 500 m from the center of the
detector. No effort is made to correct for this. 
In addition, the geographic location of IceCube move with the glacial flow of the
antarctic ice at a rate 10 m/yr no effort is made to correct for this either. 

Dependencies
============

Astronomical calculation are done using PAL.
PAL is a reimplimenation of `SLALIB <http://star-www.rl.ac.uk/docs/sun67.htx/sun67.html>`_ which is
is an old Fortran library which was translated into c, it is no longer updated,
has a weird license, and is hard to find.
`PAL <https://github.com/Starlink/pal>`_ is a newer library which is available on github and is
licensed with LGPL.
PAL is intended as a newer version of SLALIB and has an identical interface so it can fuction as a drop-in replacement.
nstructions for downloading it can be found in the `cmake tools documentaion <../cmake/tools/pal.html>`_.

See Also
========

.. toctree::
   :maxdepth: 1

   release_notes
   code_review	      
   Python API Reference </python/icecube.astro>
   C++ API Reference </doxygen/astro/index>
   IceTray Inspect Reference </inspect/astro>

