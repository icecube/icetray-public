#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

from datetime import datetime,timedelta
import pylab as plt
import matplotlib.dates as mdates
import matplotlib.ticker as ticker
from icecube.icetray import I3Units
from icecube import astro,dataclasses

lmargin,rmargin = 0.06, .02
awidth = .2
tmargin,bmargin = .01,.08
hgap,vgap = .1,0

rect_analemma  = [lmargin, bmargin, awidth, 1-bmargin-tmargin ]
rect_elevation = [lmargin+awidth+hgap, (1+bmargin-tmargin+hgap)/2, 1-lmargin-awidth-hgap-rmargin ,(1-bmargin-tmargin-hgap)/2]
rect_time      = [lmargin+awidth+hgap, bmargin, 1-lmargin-awidth-hgap-rmargin, (1-bmargin-tmargin-hgap)/2]

fig = plt.figure(figsize=(10, 6))
ax1 = fig.add_axes(rect_analemma)
ax2 = fig.add_axes(rect_elevation)
ax3 = fig.add_axes(rect_time,sharex=ax2)

days = range(367)
sun_azimuth =[]
sun_elevation = []
day = []

for n in days:
    d = datetime(2020,1,1,12)+timedelta(n-1)
    day.append(d)

    i3time = dataclasses.make_I3Time(d)
    sun = astro.I3GetSunDirection(i3time)
    sun_azimuth.append(90 - sun.azimuth/I3Units.degree)
    sun_elevation.append(90 - sun.zenith/I3Units.degree)

ax2.plot(day,sun_elevation,ls='-',marker=None)
ax2.set_ylabel("Sun Elevation")
ax2.yaxis.set_major_formatter(ticker.FormatStrFormatter(u'%d\u00b0'))
ax2.yaxis.set_minor_locator(ticker.MultipleLocator(2))
ax2.set_ylim(-26,26)
ax2.grid()

ax3.plot(day,-4*plt.array(sun_azimuth)*plt.cos(plt.radians(sun_elevation)),ls='-',marker=None)
ax3.set_ylabel("Appearent - Mean")
ax3.yaxis.set_minor_locator(ticker.MultipleLocator(1))
ax3.yaxis.set_major_formatter(ticker.FormatStrFormatter(u'%dm'))
ax3.set_ylim(-17,17)
ax3.grid()

ax3.xaxis.set_major_locator(mdates.MonthLocator([1,3,5,7,9,11]))
ax3.xaxis.set_major_formatter(mdates.DateFormatter("1 %b"))
ax3.xaxis.set_minor_locator(mdates.MonthLocator())
ax3.set_xlim(day[0],day[-1]+timedelta(1))

plt_info = ax1.plot(sun_azimuth,sun_elevation)
for n,d in enumerate(day):
    if d.day==1:
        
        slope = -(sun_azimuth[n+1]-sun_azimuth[n-1])/(sun_elevation[n+1]-sun_elevation[n-1])
        size = 1

        x1=-size/(1+slope**2)**0.5
        x2=+size/(1+slope**2)**0.5

        ax1.plot([sun_azimuth[n]+x1,sun_azimuth[n]+x2],
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
        ax1.text(txt_az,txt_el,"1 "+d.strftime("%b"),verticalalignment='center',horizontalalignment=halign)

ax1.set_xlabel("Offset From North")
ax1.set_ylabel("Sun Elevation")

ax1.xaxis.set_major_locator(ticker.MultipleLocator(5))
ax1.xaxis.set_minor_locator(ticker.MultipleLocator(1))
ax1.xaxis.set_major_formatter(ticker.FormatStrFormatter(u'%d\u00b0'))
ax1.yaxis.set_major_locator(ticker.MultipleLocator(10))
ax1.yaxis.set_minor_locator(ticker.MultipleLocator(1))
ax1.yaxis.set_major_formatter(ticker.FormatStrFormatter(u'%d\u00b0'))
ax1.set_ylim(-26,26)
ax1.set_xlim(-10,10)

plt.show()
