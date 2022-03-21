#!/usr/bin/env python3
# This example can verify that the azimuth is defined correctly with reference to the sun
# It is obvious that the sun should be at Grid South at midnight,
# Grid East at 6:00, Grid North at 12:00, and Grid West at 18:00
# the example makes a plot to verify that coordinates are defined correctly

import os
from icecube import icetray,dataio,astro,dataclasses
import numpy as np
import pylab as plt
import matplotlib.dates as mdates
import matplotlib.ticker as ticker
import matplotlib.patches as patches

fig, (ax1,ax2) = plt.subplots(1,2,figsize=[12,4.8],
                              gridspec_kw=dict(width_ratios=(1,1.1),wspace=.35,left=0.06,right=.97))

gcdfile = os.environ['I3_TESTDATA']+'/GCD/GeoCalibDetectorStatus_2016.57531_V0.i3.gz'
with dataio.I3File(gcdfile) as f:
    omgeo = f.pop_frame(Stream=icetray.I3Frame.Geometry)["I3Geometry"].omgeo

x=[]
y=[]
s=[]
for k,g in omgeo.items():
    if g.omtype != dataclasses.I3OMGeo.IceCube:
        continue
    if k.om!=1 or g.omtype != dataclasses.I3OMGeo.IceCube:
        continue
    x.append(g.position.x)
    y.append(g.position.y)
    s.append(k.string)

def drawCirc(ax,radius,centX,centY,angle_,theta2_,color_='black'):
    arc = patches.Arc([centX,centY],radius,radius,angle=angle_,
          theta1=0,theta2=theta2_,capstyle='round',linestyle='-',lw=1,color=color_)
    ax.add_patch(arc)

    endX=centX+(radius/2)*np.cos(np.deg2rad(theta2_+angle_)) #Do trig to determine end position
    endY=centY+(radius/2)*np.sin(np.deg2rad(theta2_+angle_))

    ax.add_patch(                    #Create triangle as arrow head
        patches.RegularPolygon(
            (endX, endY),            # (x,y)
            3,                       # number of vertices
            radius/9,                # radius
            np.deg2rad(angle_+theta2_),     # orientation
            color=color_
        )
    )
    ax.set_xlim([centX-radius,centY+radius]) and ax.set_ylim([centY-radius,centY+radius]) 

drawCirc(ax1,200,0,0,0,75)
ax1.text(100/2**.5,100/2**.5,"Azimuth",horizontalalignment='left',verticalalignment='bottom')
xtoe = lambda x: x/0.3048+46500
etox = lambda E: 0.3048*(E-46500)
yton = lambda y: y/0.3048+52200
ntoy = lambda N: 0.3048*(N-52200)

for i in range(len(s)):
    ax1.text(x[i],y[i],s[i],fontsize=7,horizontalalignment='center',verticalalignment='center',color='#404040')

secxax = ax1.secondary_xaxis('top', functions=(xtoe,etox))
secxax.xaxis.set_major_locator(ticker.MultipleLocator(1000))
secxax.xaxis.set_major_formatter("E {x:0,.0f}′")
secyax = ax1.secondary_yaxis('right', functions=(yton,ntoy))
secyax.yaxis.set_major_locator(ticker.MultipleLocator(1000))
secyax.yaxis.set_major_formatter("N {x:0,.0f}′")
ax1.set_aspect('equal')

lim = 800
ax1.set_xlim(-800,800)
ax1.set_ylim(-700,700)
ax1.set_xlabel('x [m]')
ax1.set_ylabel('y [m]')
sunloc = 650

for x,y,t in [(0,-620,0),(700,0,6),(0,620,12),(-660,0,18)]:
    ax1.text(x,y,f"\u2600{t:02d}:00  ",c='tab:orange',fontsize=12,
            horizontalalignment='center',verticalalignment='bottom' if x else 'center')

ax1.axvline(0,c='k',lw=1)
ax1.axhline(0,c='k',lw=1)

t0 = dataclasses.I3Time()
t0.set_utc_cal_date(2021,3,20,0,0,0,0)
frac_day =  np.linspace(0,1,1001)

mjd = t0.mod_julian_day + frac_day
_,fpa_azimuth = astro.equa_to_dir(0.,0.,mjd)
_,sun_azimuth = astro.sun_dir(mjd)

tt = plt.date2num(t0.date_time) + frac_day

plt.plot(tt,np.rad2deg(fpa_azimuth),ls='-',label='First Point of Ares')
plt.plot(tt,np.rad2deg(sun_azimuth),ls='--',label='Sun')

ax2.xaxis.set_major_locator(mdates.AutoDateLocator())
ax2.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
ax2.xaxis.set_minor_locator(mdates.HourLocator())

ax2.yaxis.set_major_locator(ticker.MultipleLocator(45))
ax2.yaxis.set_major_formatter('{x:0.0f}°')
ax2.yaxis.set_minor_locator(ticker.MultipleLocator(15))

secax = ax2.secondary_yaxis('right')
secax.yaxis.set_major_locator(ticker.MultipleLocator(90))
secax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x,y:['E','N','W','S'][(int(x)%360)//90]))
ax2.set_ylabel("Azimuth")
ax2.set_xlabel("UTC Time")
ax2.set_xlim(tt[0],tt[-1])
ax2.set_ylim(0,360)

ax2.set_title('Vernal Equinox '+t0.date_time.date().isoformat())
ax2.legend()
ax2.grid()
plt.show()
