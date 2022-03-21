#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

import pylab as plt
import numpy as np
from matplotlib.ticker import FormatStrFormatter
from matplotlib.dates import DateFormatter,AutoDateLocator

from datetime import datetime
from icecube import dataclasses,astro
from scipy.optimize import fminbound

def moon_alt(x):
    return 90-np.rad2deg(astro.moon_dir(x)[0])

i3t0 =dataclasses.I3Time()
i3t0.set_utc_cal_date(2020,1,2,0,0,0,0.0)
mjd0 = i3t0.mod_julian_day_double

T = 27.322
NPeriods = 252
toff = np.linspace(0, T, 100)

fig1, ax1 = plt.subplots(1,1)
mjd_max = []

for i in range(NPeriods):
    ax1.plot(toff,moon_alt(mjd0+i*T+toff))
    mmax = fminbound(lambda x: astro.moon_dir(x)[0], mjd0+(i+0.5)*T, mjd0+(i+1)*T)
    amax = moon_alt(mmax)
    mjd_max.append(mmax)

ax1.set_xlabel("Phase of Moon [Days]")
ax1.set_xlim(0, T)
ax1.set_ylabel("Moon Altitude")
ax1.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax1.grid()

fig2,ax2=plt.subplots(1,1)
plt0 = plt.date2num(i3t0.date_time)
plt_max = np.array(mjd_max)-mjd0+plt0

ax2.plot(plt_max,moon_alt(mjd_max))
toff = np.linspace(0,NPeriods*T,10000)
alts = moon_alt(toff+mjd0)

ax2.xaxis.set_major_formatter( DateFormatter("%Y"))
ax2.xaxis.set_major_locator( AutoDateLocator() )
ax2.set_xlim(datetime(2020,1,1,0,0,0),plt_max[-1])
ax2.set_xlabel("Year")
ax2.set_ylabel("Maximum Moon Altitude")
ax2.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax2.grid()

fig3,ax3=plt.subplots(1,1)
moon_zenith,moon_azimuth = astro.moon_dir(mjd_max)
ra,dec = astro.dir_to_equa(moon_zenith,moon_azimuth,mjd_max)
ax3.plot(np.rad2deg(ra),np.rad2deg(dec))
ax3.set_xlim(288,253)
ax3.set_ylim(-28,-17)
ax3.xaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax3.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax3.set_title("Moon Maximum Altitude")
ax3.set_xlabel("Right ascension")
ax3.set_ylabel("Declination")
ax3.grid()

plt.show()
