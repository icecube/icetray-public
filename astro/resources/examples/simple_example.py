#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
Simple example demonstraing all the functions in astro

"""

from icecube.icetray import I3Units
from icecube import dataclasses,astro

time = dataclasses.I3Time()

time.set_mod_julian_time_double(56819.20444852863)

#calculate the position of the moon
moon_direction = astro.I3GetMoonDirection(time)
print ("At {} the moon will be at zenith={:8.4f} deg, azimuth={:7.4f} deg"
       .format(str(time),
               moon_direction.zenith/I3Units.degree,
               moon_direction.azimuth/I3Units.degree,
               ))

print

#calculate the position of the sun
sun_direction = astro.I3GetSunDirection(time)
print ("At {} the Sun will be at zenith={:8.4f} deg, azimuth={:7.4f} deg"
       .format(str(time),
               sun_direction.zenith/I3Units.degree,
               sun_direction.azimuth/I3Units.degree,
               ))

print

#This is where the crab lives on the sky
crab_position = astro.I3Equatorial(83.63308*I3Units.degree,
                                   22.01450*I3Units.degree)
print ("The Crab Nebula is located at RA = {:8.4f} deg, Dec ={:+7.4f} deg (J2000)"
       .format(crab_position.ra/I3Units.degree,
               crab_position.dec/I3Units.degree,
               ))


#calculate the location of the crab in galactic coordinates
crab_galactic = astro.I3GetGalacticFromEquatorial(crab_position)
print ("Which Means its galactic coordinates are l={:+8.4f} deg, b={:+7.4f} deg"
       .format(crab_galactic.l/I3Units.degree,
               crab_galactic.b/I3Units.degree,
               ))


#calculat the position of the crab nebula in local coordinates
crab_direction = astro.I3GetDirectionFromEquatorial(crab_position,time)
print ("At {} the Crab will be at zenith={:8.4f} deg, azimuth={:7.4f} deg"
       .format(str(time),
               crab_direction.zenith/I3Units.degree,
               crab_direction.azimuth/I3Units.degree,
               ))

print

#the center of the galaxy is easy to remember in galactic coordinates
galaxy_center = astro.I3Galactic(0,0)
print ("The Galactic Center's galactic coordinates are l={:+8.4f} deg, b={:+7.4f} deg"
       .format(galaxy_center.l/I3Units.degree,
               galaxy_center.b/I3Units.degree,
               ))


#convert to equatorial coordinates
gc_equatorial = astro.I3GetEquatorialFromGalactic(galaxy_center)
print ("Which means its equatorial coordinates are ra={:8.4f} deg, dec={:+7.4f} deg"
       .format(
               gc_equatorial.ra/I3Units.degree,
               gc_equatorial.dec/I3Units.degree,
               ))

#calculat the position of the galactic in local coordinates
gc_direction = astro.I3GetDirectionFromEquatorial(gc_equatorial,time)
print ("At {} the Crab will be at zenith={:8.4f} deg, azimuth={:7.4f} deg"
       .format(str(time),
               gc_direction.zenith/I3Units.degree,
               gc_direction.azimuth/I3Units.degree,
               ))

print

#calculat the position of the crab nebula in local coordinates
muon_direction = dataclasses.I3Direction(101.4572*I3Units.degree,
                                         312.5199*I3Units.degree)
print("At {} a very bright muon was observed by IceCube at zenith={:8.4f} deg, azimuth={:7.4f} deg"
       .format(str(time),
               muon_direction.zenith/I3Units.degree,
               muon_direction.azimuth/I3Units.degree,
               ))

muon_equatorial = astro.I3GetEquatorialFromDirection(muon_direction,time)                    
print("The The Muon came from of RA = {:8.4f} deg, Dec ={:+7.4f} deg (J2000)"
       .format(muon_equatorial.ra/I3Units.degree,
               muon_equatorial.dec/I3Units.degree,
       ))

muon_galactic = astro.I3GetGalacticFromEquatorial(muon_equatorial)
print("Which Means its galactic coordinates are l={:+8.4f} deg, b={:+7.4f} deg"
      .format(muon_galactic.l/I3Units.degree,
              muon_galactic.b/I3Units.degree,
      ))
