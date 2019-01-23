#!/usr/bin/env python
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

from datetime import datetime,timedelta
import pylab as plt
from matplotlib.ticker import FormatStrFormatter
from icecube.icetray import I3Units
from icecube import astro,dataclasses

days = range(367)

sun_azimuth =[]
sun_elevation = []
day = []
for n in days:

    d = datetime(2000,1,1,12)+timedelta(n-1)
    day.append(d)
    i3time = dataclasses.make_I3Time(d)

    sun = astro.I3GetSunDirection(i3time)

    sun_azimuth.append(90 - sun.azimuth/I3Units.degree)
    sun_elevation.append(90 - sun.zenith/I3Units.degree)

plt.plot_date(day,
              -4*plt.array(sun_azimuth)*plt.cos(plt.radians(sun_elevation)),
              ls='-',marker=None
)
plt.title("Equation of Time")
plt.ylabel("Sundial Faster [minutes]")

plt.figure()
plt.plot_date(day,sun_elevation,ls='-',marker=None)
plt.ylabel("Sun Elevation")
plt.title("Sun Elevation")
plt.gca().yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))


plt.figure()
ax = plt.gca()
plt_info = ax.plot(sun_azimuth,sun_elevation)

for n,d in enumerate(day):
    if d.day==1:
        
        slope = -(sun_azimuth[n+1]-sun_azimuth[n-1])/(sun_elevation[n+1]-sun_elevation[n-1])

        size = 1

        x1=-size/(1+slope**2)**0.5
        x2=+size/(1+slope**2)**0.5

        plt.plot([sun_azimuth[n]+x1,sun_azimuth[n]+x2],
                 [ sun_elevation[n] + slope*x1,
                   sun_elevation[n] + slope*x2,
                   ],
                 c=plt_info[0].get_c()
                 )

        if sun_azimuth[n]<0:
            halign = 'right'
            text_az = sun_azimuth[n] - 1
            x = -1.5*size/(1+slope**2)**0.5
            txt_az = sun_azimuth[n]+x
            txt_el = sun_elevation[n]+slope*x
            
        else:
            halign = 'left'
            text_az = sun_azimuth[n] + 1
            x = 1.5*size/(1+slope**2)**0.5
            txt_az = sun_azimuth[n]+x
            txt_el = sun_elevation[n]+slope*x


        plt.annotate("1 "+d.strftime("%b"),
                     xy=(sun_azimuth[n],sun_elevation[n]),
                     xytext=(txt_az,txt_el+0.5),
                     textcoords='data',
                     horizontalalignment=halign,
                     verticalalignment='center',
                     )
        
ax.set_xlabel("Azimuth")
ax.set_ylabel("Elevation")
ax.xaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax.yaxis.set_major_formatter(FormatStrFormatter(u'%d\u00b0'))
ax.set_aspect('equal', 'datalim')
ax.set_title("Analemma")

plt.show()




