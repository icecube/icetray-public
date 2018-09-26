#!/usr/bin/env python
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

import pylab as plt
from matplotlib.ticker import MultipleLocator,FormatStrFormatter
from matplotlib.dates import WeekdayLocator,SU,DateFormatter
from datetime import datetime,timedelta
from icecube.icetray import I3Units
from icecube import dataclasses,astro

i3time = dataclasses.I3Time()
i3time.set_utc_cal_date(2016,1,1,0,0,0,0.0) # 1 Jan 2016 00:00

i3_moon_alt=[]
i3_moon_az=[]
times = []

for n in range(10000):
    
    i3time+= 300*I3Units.second

    moon_dir = astro.I3GetMoonDirection(i3time)
    i3_moon_alt.append( moon_dir.zenith/I3Units.degree)
    i3_moon_az.append( moon_dir.azimuth/I3Units.degree)
    times.append(datetime.fromtimestamp(i3time.unix_time))


fig, (ax1,ax2) = plt.subplots(2,sharex=True)
ax1.set_title("IceCube Coordinates for the Moon")
ax1.plot_date(times,i3_moon_alt,marker=None,ls='-',label = "I3")
ax1.yaxis.set_major_locator( MultipleLocator(10))
ax1.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax1.set_ylabel("Zenith")


ax2.plot_date(times,i3_moon_az,marker=None,ls='-',label = "I3")
ax2.set_ylabel("IceCube Azimuth")
ax2.set_ylim(0,360)
ax2.xaxis.set_major_formatter( DateFormatter("%d %b %Y"))
ax2.xaxis.set_major_locator( WeekdayLocator(byweekday=SU) )
ax2.yaxis.set_major_locator( MultipleLocator(90))
ax2.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))

fig.subplots_adjust(hspace=0)



plt.show()
