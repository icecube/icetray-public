# -*- coding: utf-8 -*-
# 
# copyright  (C) 2015
# The Icecube Collaboration
# 
# @date August 2015
# @author Kevin Meagher
# 

from icecube.load_pybindings import load_pybindings
load_pybindings(__name__,__path__)
del load_pybindings

import numpy as np
from icecube.icetray import I3Units
from icecube import dataclasses

@np.vectorize
def dir_to_equa(zenith,azimuth,mjd):
    """
    Get the equatorial coordinates (right ascension and 
    declination) of an IceCube event (`zenith` 
    and `azimuth`) at a given time (`mjd`).
    
    Parameters
    ----------
    zenith : array_like 
       IceCube zenith of the direction
    azimuth: array_like 
       IceCube azimuth of the direction
    mjd: array_like 
       Modified julian date of the event as as floating point

    Returns
    -------
    ra : array_like 
      Right Ascension in J2000
    dec : array_like 
      Declination in J2000

    """
    i3dir = dataclasses.I3Direction(zenith,azimuth)
    i3time = dataclasses.I3Time()
    i3time.set_mod_julian_time_double(mjd)
    eq = I3GetEquatorialFromDirection(i3dir,i3time)
    return eq.ra,eq.dec

@np.vectorize
def equa_to_dir(ra,dec,mjd):
    """
    Get the IceCube `zenith` and `azimuth` of an
    equatorial coordinate (Right Ascension and Declination)
    at a given time `mjd`.

    Warnings
    --------

    This function is significantly slower than :py:func:`dir_to_equa`, 
    it is highly recommended that if you need to perfrom a significant number
    of calculations that you go from local to equatorial coordinates and 
    perform the analyses in equatorial coordinates, rather than the reverse.
    
    Parameters
    ----------
    ra : array_like 
       Right Ascension (J2000) of the event
    dec : array_like 
       Declination (J2000) of the event
    mjd: array_like float
       Modified julian date of the event as a floating point

    Returns
    -------
    zenith : array_like
      IceCube zenith direction 
    azimuth : array_like
      IceCube azimuthal direction 
    """
    eq = I3Equatorial(ra,dec)
    i3time = dataclasses.I3Time()
    i3time.set_mod_julian_time_double(mjd)
    i3dir = I3GetDirectionFromEquatorial(eq,i3time)
    return i3dir.zenith,i3dir.azimuth

@np.vectorize
def sun_dir(mjd):
    """
    Get the location of the Sun in IceCube local coordinates
    `zenith` and `azimuth` for a given time in `mjd`
    """
    
    i3time = dataclasses.I3Time()
    i3time.set_mod_julian_time_double(mjd)
    i3dir = I3GetSunDirection(i3time)
    return i3dir.zenith,i3dir.azimuth

@np.vectorize
def moon_dir(mjd):
    """
    Get the location of the Moon in IceCube local coordinates
    `zenith` and `azimuth` for a given time in `mjd`
    """
    
    i3time = dataclasses.I3Time()
    i3time.set_mod_julian_time_double(mjd)
    i3dir = I3GetMoonDirection(i3time)
    return i3dir.zenith,i3dir.azimuth


@np.vectorize
def equa_to_gal(ra,dec):
    """
    Get the Galactic coordinates `l` and `b` of an
    equatorial coordinate (Right Ascension and Declination).
    
    Parameters
    ----------
    ra : array_like 
       Right Ascension (J2000) of the event
    dec : array_like 
       Declination (J2000) of the event

    Returns
    -------
    l : array_like
      galactic longitude
    b : array_like
      galactic latitude
    """
    eq = I3Equatorial(ra,dec)
    gal = I3GetGalacticFromEquatorial(eq)
    return gal.l,gal.b

@np.vectorize
def gal_to_equa(l,b):
    """
    Get the equatorial coordinates (Right Ascension 
    and Declination) from  Galactic coordinates 
    `l` and `b`.

    Parameters
    ----------
    l : array_like
      galactic longitude
    b : array_like
      galactic latitude

    Returns
    -------
    ra : array_like 
       Right Ascension (J2000) of the event
    dec : array_like 
       Declination (J2000) of the event


    """
    gal = I3Galactic(l,b)
    eq = I3GetEquatorialFromGalactic(gal)
    return eq.ra,eq.dec

@np.vectorize
def gal_to_supergal(l,b):
    """
    Get the supergalactic coordinates 'SGL' and 'SGB'
    from galactic coordinates 'l' and 'b'

    Parameters
    ----------
    l : array_like
      galactic longitude
    b : array_like
      galactic latitude

    Returns
    -------
    l : array_like
      supergalactic longitude (de Vaucouleurs) of the event
    b : array_like
      supergalactic latitude (de Vaucouleurs) of the event


    """
    gal = I3Galactic(l,b)
    sg = I3GetSuperGalacticFromGalactic(gal)
    return sg.l, sg.b

@np.vectorize
def supergal_to_gal(l,b):
    """
    Get the galactic coordinates 'l' and 'b'
    from supergalactic coordinates 'SGL' and 'SGB'

    Parameters
    ----------
    l : array_like
      supergalactic longitude
    b : array_like
      supergalactic latitude

    Returns
    -------
    l : array_like
      galactic longitude (IAU 1958) of the event
    b : array_like
      galactic latitude (IAU 1958) of the event


    """
    sg = I3SuperGalactic(l,b)
    gal = I3GetGalacticFromSuperGalactic(sg)
    return gal.l, gal.b

@np.vectorize
def equa_to_supergal(ra,dec):
    """
    Get the supergalactic coordinates 'SGL' and 'SGB'
    from equatorial coordinates 'ra' and 'dec'

    Parameters
    ----------
    ra : array_like
      right ascension
    dec : array_like
      declination

    Returns
    -------
    l : array_like
      supergalactic longitude (de Vaucouleurs) of the event
    b : array_like
      supergalactic latitude (de Vaucouleurs) of the event


    """
    eq = I3Equatorial(ra, dec)
    sg = I3GetSuperGalacticFromEquatorial(eq)
    return sg.l, sg.b

@np.vectorize
def supergal_to_equa(l,b):
    """
    Get the equatorial coordinates `ra` and `dec`
    from supergalactic coordinates `SGL` and `SGB`

    Parameters
    ----------
    l : array_like
      supergalactic longitude (de Vaucouleurs) of the event
    b : array_like
      supergalactic latitude (de Vaucouleurs) of the event

    Returns
    -------
    ra : array_like
      right ascension
    dec : array_like
      declination

    """
    sg = I3SuperGalactic(l, b)
    eq = I3GetEquatorialFromSuperGalactic(sg)
    return eq.ra, eq.dec


def angular_distance(lon1,lat1,lon2,lat2):
    """
    calculate the angular distince along the great circle 
    on the surface of a shpere between the points
    (`lon1`,`lat1`) and (`lon2`,`lat2`) 

    This function Works for equatorial coordinates 
    with right ascension as longitude and declination 
    as latitude. This function uses the Vincenty formula
    for calculating the distance.

    Parameters
    ----------
    lon1 : array_like
      longitude of first point in radians
    lat1 : array_like
      latitude of the first point in radians
    lon2 : array_like
      longitude of second point in radians
    lat2 : array_like 
      latitude of the second point in radians

    """

    c1 = np.cos(lat1)
    c2 = np.cos(lat2)
    s1 = np.sin(lat1)
    s2 = np.sin(lat2)
    sd = np.sin(lon2-lon1)
    cd = np.cos(lon2-lon1)
    
    return np.arctan2(
        np.hypot(c2*sd,c1*s2-s1*c2*cd),
        s1*s2+c1*c2*cd
        )

def fractional_mjd(mjd_day,mjd_sec,mjd_ns):
    """
    This is a convenience function to convert the MJD information provided by
    the standard tableio booking to a fractional MJD.

    Parameters
    ----------
    mjd_day : array_like
      Modified julian day (number of days since  Nov 17, 1858)
    mjd_sec : array_like
      Seconds since the start of the UTC day
    mjd_ns : array_like
      number nanoseconds since the start of UTC second  
    """

    return mjd_day + mjd_sec/86400. + mjd_ns/8.64e13
    
def tables_to_equa(particle_table,event_header_table):
    """
    Get the equatorial coordinates (right ascension and 
    declination) of IceCubes Events from tables writen by 
    tableio. Works with hdf5 tables written by hdfwriter
    and read by pytables, h5py, or pandas
    
    Parameters
    ----------
    particle_table : table
       Table containing Zenith and Azimuth of particles
    event_header_table : table
       Table containing the start time of the events 

    Returns
    -------
    ra : array_like 
      Right Ascension in J2000
    dec : array_like 
      Declination in J2000

    Example
    -------

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
       
    """
    if hasattr(particle_table,"cols"):
        zenith = particle_table.cols.zenith[:]
        azimuth = particle_table.cols.azimuth[:]
    else:
        zenith = particle_table["zenith"]
        azimuth = particle_table["azimuth"]

    if hasattr( event_header_table,"cols"):
        mjd_day = event_header_table.cols.time_start_mjd_day[:]
        mjd_sec = event_header_table.cols.time_start_mjd_sec[:]
        mjd_ns  = event_header_table.cols.time_start_mjd_ns[:]
    else:
        mjd_day = event_header_table["time_start_mjd_day"]
        mjd_sec = event_header_table["time_start_mjd_sec"]
        mjd_ns  = event_header_table["time_start_mjd_ns"]
        
    mjd = fractional_mjd(mjd_day,mjd_sec,mjd_ns)

    return dir_to_equa(zenith,azimuth,mjd)
        
