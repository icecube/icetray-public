#!/usr/bin/env python3
import datetime
import numpy as np
import pylab as plt

#crab
src_ra=83.63308
src_dec=22.01450

#TXS
src2_ra=77.43
src2_dec=5.72

#SGR*
src3_ra=255.76
src3_dec=-29.001

mjd=59000.
times = mjd + np.linspace(0,1,1000)

import astropy.units as u
from astropy.time import Time
from astropy.coordinates import SkyCoord, EarthLocation, AltAz

ap_coords = SkyCoord(ra=src_ra*u.degree, dec=src_dec*u.degree, frame='icrs')
ap2_coords = SkyCoord(ra=src2_ra*u.degree, dec=src2_dec*u.degree, frame='icrs')
ap3_coords = SkyCoord(ra=src3_ra*u.degree, dec=src3_dec*u.degree, frame='icrs')

icecube = EarthLocation(lat=-89.9944*u.deg, lon=-62.6081*u.deg, height=0*u.m)
ap_times = Time(times,format='mjd',scale='ut1')

ap_altaz = ap_coords.transform_to(AltAz(obstime=ap_times,location=icecube))
ap2_altaz = ap2_coords.transform_to(AltAz(obstime=ap_times,location=icecube))
ap3_altaz = ap3_coords.transform_to(AltAz(obstime=ap_times,location=icecube))
ap_zenith=(90*u.degree - ap_altaz.alt).degree
ap2_zenith=(90*u.degree - ap2_altaz.alt).degree
ap3_zenith=(90*u.degree - ap3_altaz.alt).degree
ap_azimuth=(90*u.degree - ap_altaz.az - icecube.lon).wrap_at(360*u.degree).degree
ap2_azimuth=(90*u.degree - ap2_altaz.az - icecube.lon).wrap_at(360*u.degree).degree
ap3_azimuth=(90*u.degree - ap3_altaz.az - icecube.lon).wrap_at(360*u.degree).degree

from icecube import astro
from icecube.icetray import I3Units

mjds =  mjd + np.linspace(0,1,1000)
i3zenith,i3azimuth = astro.equa_to_dir(
    src_ra*I3Units.degree,
    src_dec*I3Units.degree,
    mjds)

i3zenith_2,i3azimuth_2 = astro.equa_to_dir(
    src2_ra*I3Units.degree,
    src2_dec*I3Units.degree,
    mjds)

i3zenith_3,i3azimuth_3 = astro.equa_to_dir(
    src3_ra*I3Units.degree,
    src3_dec*I3Units.degree,
    mjds)

f, ([ax1, ax2], [ax3, ax4]) = plt.subplots( 2,2,sharex=True)
ax1.plot(ap_zenith)
ax1.plot(i3zenith/I3Units.degree)
ax1.plot(ap2_zenith)
ax1.plot(i3zenith_2/I3Units.degree)
ax1.plot(ap3_zenith)
ax1.plot(i3zenith_3/I3Units.degree)
ax1.set_ylim(0,180)
ax1.set_ylabel("Zenith(deg)")

ax2.plot(ap_azimuth)
ax2.plot(i3azimuth/I3Units.degree)
ax2.plot(ap2_azimuth)
ax2.plot(i3azimuth_2/I3Units.degree)
ax2.plot(ap3_azimuth)
ax2.plot(i3azimuth_3/I3Units.degree)
ax2.set_ylabel("Azimuth(deg)")

ax3.plot(ap_zenith - i3zenith/I3Units.degree)
ax3.plot(ap2_zenith - i3zenith_2/I3Units.degree)
ax3.plot(ap3_zenith - i3zenith_3/I3Units.degree)
ax3.set_ylabel("Delta Zen(deg)")

ax4.plot(ap_azimuth - i3azimuth/I3Units.degree)
ax4.plot(ap2_azimuth - i3azimuth_2/I3Units.degree)
ax4.plot(ap3_azimuth - i3azimuth_3/I3Units.degree)
ax4.set_ylabel("Delta Azi(deg)")

plt.show()
