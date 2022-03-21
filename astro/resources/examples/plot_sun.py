#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

import pylab as plt
from matplotlib.ticker import MultipleLocator,FormatStrFormatter
from matplotlib.dates import WeekdayLocator,SU,DateFormatter,HourLocator
from datetime import datetime,timedelta
from icecube.icetray import I3Units
from icecube import dataclasses,astro

i3time = dataclasses.I3Time()
i3time.set_utc_cal_date(2017,9,22,0,0,0,0.0) # atumnal equinox
dstr= i3time.get_utc_string("%d %B %Y")
i3_sun_zenith=[]
i3_sun_azimuth=[]
times = []

N=10000
for n in range(N+1):
    sun_dir = astro.I3GetSunDirection(i3time)
    i3_sun_zenith.append( sun_dir.zenith/I3Units.degree)
    i3_sun_azimuth.append( sun_dir.azimuth/I3Units.degree)
    times.append(i3time.date_time)
    i3time+= I3Units.day/N

fig, (ax1,ax2) = plt.subplots(2,sharex=True)
ax1.set_title("IceCube Coordinates for the Sun "+dstr)
ax1.plot_date(times,i3_sun_zenith,marker=None,ls='-',label = "I3")
ax1.yaxis.set_major_locator( MultipleLocator(0.1))
ax1.yaxis.set_major_formatter(FormatStrFormatter(u'%4.1f\u00b0'))
ax1.set_ylabel("Zenith")
ax1.set_ylim(min(i3_sun_zenith),max(i3_sun_zenith))

ax2.plot_date(times,i3_sun_azimuth,marker=None,ls='-',label = "I3")
ax2.set_ylabel("Azimuth")
ax2.set_ylim(0,360)
ax2.xaxis.set_major_formatter( DateFormatter("%H:%M"))
ax2.xaxis.set_major_locator( HourLocator(interval=3))
ax2.yaxis.set_major_locator( MultipleLocator(90))
ax2.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
fig.subplots_adjust(hspace=0)

plt.show()
