#!/usr/bin/env python3

import os, sys
import matplotlib.pyplot as plt

from icecube import icetray
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 
from I3Tray import *

from icecube.phys_services import I3ScaleCalculator

## Let's grab a fairly ordinary IC86 GCD file
TESTDATA = os.path.expandvars("$I3_TESTDATA")
I3_SRC = os.path.expandvars("$I3_SRC")
gcdfile = TESTDATA+"/GCD/GeoCalibDetectorStatus_2020.Run134142.Pass2_V0.i3.gz"

## Load the I3Geometry from that file
my_gcd = phys_services.I3GCDFileGeometryService(gcdfile)
mytime = dataclasses.I3Time(2020, 158082172000000000)
geo = my_gcd.get_geometry(mytime)

## Create a few I3ScaleCalculators
sc_ic79_smooth = I3ScaleCalculator(geo, I3ScaleCalculator.IC79_SMOOTH, I3ScaleCalculator.IT73_SMOOTH)
sc_ic79_strict = I3ScaleCalculator(geo, I3ScaleCalculator.IC79_STRICT, I3ScaleCalculator.IT73_STRICT)
sc_ic86_smooth = I3ScaleCalculator(geo, I3ScaleCalculator.IC86_SMOOTH, I3ScaleCalculator.IT81_SMOOTH)
sc_ic86_strict = I3ScaleCalculator(geo, I3ScaleCalculator.IC86_STRICT, I3ScaleCalculator.IT81_STRICT)
sc_subarrays_small = I3ScaleCalculator(geo, I3ScaleCalculator.DEEPCORE_BELOWDUST, I3ScaleCalculator.IT_INFILL_STA2_STRICT)
sc_subarrays_wider = I3ScaleCalculator(geo, I3ScaleCalculator.DEEPCORE_ALL, I3ScaleCalculator.IT_INFILL_TRIANGLE)
sc_subarrays_oval = I3ScaleCalculator(geo, I3ScaleCalculator.IC_EMPTY, I3ScaleCalculator.IT_INFILL_STA2_BIGOVAL)

## ---------------- Create an ICETOP figure -------------------
f1 = plt.figure(1, figsize=[9,9])
xx = []
yy = []
labels = []
#sg = geo.stationgeo
for ist in range(1,82):
    tankposA = geo.tankgeo(dataclasses.TankKey(ist,dataclasses.TankKey.TankA)).position
    xx.append(tankposA.x)
    yy.append(tankposA.y)
    labels.append(str(ist)+'A')
    tankposB = geo.tankgeo(dataclasses.TankKey(ist,dataclasses.TankKey.TankB)).position
    xx.append(tankposB.x)
    yy.append(tankposB.y)
    labels.append(str(ist)+'B')

plt.plot(xx, yy, 'ko')
plt.xlabel("X (m)")
plt.ylabel("Y (m)")

def plot_one_station_boundary(xx, yy, bstations, colorletter, label):
  ## Plot the boundaries
  xb = []
  yb = []
  zflat = 0
  print(bstations)
  for ib in bstations:
    # assuming they're all there, and in order
    i1 = 2*(ib-1)
    xb.append((xx[i1]+xx[i1+1])/2)
    yb.append((yy[i1]+yy[i1+1])/2)
  # complete the loop
  xb.append(xb[0])
  yb.append(yb[0])
  plt.plot(xb, yb, colorletter, label=label)

#plot_one_station_boundary(xx, yy, sc_ic79_smooth.get_outer_stations(), 'r-', "IT73 (smooth)")
#plot_one_station_boundary(xx, yy, sc_ic79_strict.get_outer_stations(), 'b-', "IT73 (strict)")
plot_one_station_boundary(xx, yy, sc_ic86_smooth.get_outer_stations(), 'r-', "IT81 (smooth)")
plot_one_station_boundary(xx, yy, sc_ic86_strict.get_outer_stations(), 'b--', "IT81 (strict)")
plot_one_station_boundary(xx, yy, sc_subarrays_small.get_outer_stations(), 'c-', "IceTop Infill (2-station strict)")
plot_one_station_boundary(xx, yy, sc_subarrays_oval.get_outer_stations(), 'y-', "IceTop Infill (2-station big oval)")
plot_one_station_boundary(xx, yy, sc_subarrays_wider.get_outer_stations(), 'g-', "IceTop Infill (triangle)")

plt.legend(loc="upper left")

## Also put the labels on there
for l, x, y in zip(labels, xx, yy):
  if (l[-1] == 'A'):
      lst = l[0:-1]
      plt.annotate(lst,xy=(x, y), xytext=(-3, 3), textcoords='offset points', ha='right', va='bottom')
      #bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
      #arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

#f1.show()
f1.savefig(I3_SRC+"/phys-services/resources/docs/I3ScaleCalculator_icetop_boundaries.png", facecolor='white')

##  --------------- Create an ICECUBE figure ------------------
f2 = plt.figure(2, figsize=[9,9])
xx = []
yy = []
labels = []
#sg = geo.stationgeo
for ist in range(1,87):
    strpos = geo.omgeo[icetray.OMKey(ist,30)].position
    xx.append(strpos.x)
    yy.append(strpos.y)
    labels.append(str(ist))

plt.plot(xx, yy, 'ko')
plt.xlabel("X (m)")
plt.ylabel("Y (m)")

def plot_one_string_boundary(xx, yy, bstrings, colorletter, label):
  ## Plot the boundaries
  xb = []
  yb = []
  zflat = 0
  print(bstrings)
  for ib in bstrings:
    # assuming they're all there, and in order
    i1 = ib-1
    xb.append(xx[i1])
    yb.append(yy[i1])
  # complete the loop
  xb.append(xb[0])
  yb.append(yb[0])
  plt.plot(xb, yb, colorletter, label=label)

#plot_one_string_boundary(xx, yy, sc_ic79_smooth.get_outer_strings(), 'r-', "IC79 (smooth)")
#plot_one_string_boundary(xx, yy, sc_ic79_strict.get_outer_strings(), 'b-', "IC79 (strict)")
plot_one_string_boundary(xx, yy, sc_ic86_smooth.get_outer_strings(), 'r-', "IC86 (smooth)")
plot_one_string_boundary(xx, yy, sc_ic86_strict.get_outer_strings(), 'b--', "IC86 (strict)")
plot_one_string_boundary(xx, yy, sc_subarrays_small.get_outer_strings(), 'g-', "DeepCore")

plt.legend(loc="upper left")

## Also put the labels on there
for l, x, y in zip(labels, xx, yy):
    plt.annotate(l,xy=(x, y), xytext=(-3, 3), textcoords='offset points', ha='right', va='bottom')

#f2.show()
f2.savefig(I3_SRC+"/phys-services/resources/docs/I3ScaleCalculator_icecube_boundaries.png", facecolor='white')


