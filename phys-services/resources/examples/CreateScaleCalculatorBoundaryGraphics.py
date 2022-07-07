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
#sc_ic79_smooth = I3ScaleCalculator(geo, I3ScaleCalculator.IC79_SMOOTH, I3ScaleCalculator.IT73_SMOOTH)
#sc_ic79_strict = I3ScaleCalculator(geo, I3ScaleCalculator.IC79_STRICT, I3ScaleCalculator.IT73_STRICT)
sc_ic86_smooth = I3ScaleCalculator(geo, I3ScaleCalculator.IC86_SMOOTH, I3ScaleCalculator.IT81_SMOOTH)
sc_ic86_strict = I3ScaleCalculator(geo, I3ScaleCalculator.IC86_STRICT, I3ScaleCalculator.IT81_STRICT)
sc_subarrays_small = I3ScaleCalculator(geo, I3ScaleCalculator.DEEPCORE_BELOWDUST, I3ScaleCalculator.IT_INFILL_STA2_STRICT)
sc_subarrays_wider = I3ScaleCalculator(geo, I3ScaleCalculator.DEEPCORE_ALL, I3ScaleCalculator.IT_INFILL_TRIANGLE)
sc_subarrays_oval = I3ScaleCalculator(geo, I3ScaleCalculator.IC_EMPTY, I3ScaleCalculator.IT_INFILL_STA2_BIGOVAL)

## ---------------- Create an ICETOP figure (top view) -------------------
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

#f1.show()
f1.savefig(I3_SRC+"/phys-services/resources/docs/plots_scalecalculator/I3ScaleCalculator_icetop_boundaries.png", facecolor='white')

##  --------------- Create an ICECUBE figure (top view) ------------------
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
f2.savefig(I3_SRC+"/phys-services/resources/docs/plots_scalecalculator/I3ScaleCalculator_icecube_boundaries.png", facecolor='white')


##  --------------- Create an 3-D ICECUBE figure (oblique view) ------------------
from mpl_toolkits import mplot3d
f3 = plt.figure(3, figsize=[9,9])
ax = plt.subplot(1, 1, 1, projection='3d')
ax.view_init(20, -60)

def draw_3d_inice_array(bstrings, colorletter, label, topdom=1, bottomdom=60):
  n = len(bstrings)
  
  ## Plot the boundaries
  xb = []
  yb = []
  ztop = []
  zbottom = []
  print(bstrings)
  for ib in bstrings:
    # assuming they're all there, and in order
    #ist = ib-1
    middom = int((topdom+bottomdom)/2)
    xb.append(geo.omgeo[icetray.OMKey(ib,middom)].position.x)
    yb.append(geo.omgeo[icetray.OMKey(ib,middom)].position.y)
    ztop.append(geo.omgeo[icetray.OMKey(ib,topdom)].position.z)
    zbottom.append(geo.omgeo[icetray.OMKey(ib,bottomdom)].position.z)
  
  # complete the loop
  xb.append(xb[0])
  yb.append(yb[0])
  ztop.append(ztop[0])
  zbottom.append(zbottom[0])
    
  for i in range(0, n):
    if i==0:
      ax.plot3D([xb[i], xb[i]], [yb[i], yb[i]], [ztop[i], zbottom[i]], colorletter, label=label)  # verticals (with label)
    else:
      ax.plot3D([xb[i], xb[i]], [yb[i], yb[i]], [ztop[i], zbottom[i]], colorletter)  # without the label
    ax.plot3D([xb[i], xb[i+1]], [yb[i], yb[i+1]], [ztop[i], ztop[i+1]], colorletter)  # top boundary
    ax.plot3D([xb[i], xb[i+1]], [yb[i], yb[i+1]], [zbottom[i], zbottom[i+1]], colorletter) # bottom boundary
    

draw_3d_inice_array(sc_ic86_smooth.get_outer_strings(), 'r-', "IC86 (smooth)", 1, 60)
draw_3d_inice_array(sc_ic86_strict.get_outer_strings(), 'b--', "IC86 (strict)", 1, 60)
draw_3d_inice_array(sc_subarrays_wider.get_outer_strings(), 'g-', "DeepCore (all)", 19, 60)
draw_3d_inice_array(sc_subarrays_small.get_outer_strings(), 'c--', "DeepCore (below dust)", 40, 60)

ax.set_xlabel("X (m)")
ax.set_ylabel("Y (m)")
ax.set_zlabel("Z (m)")


plt.legend(loc="upper left")

## Also put some labels on there, for which DOM's are used for "z"
ax.text(-650, -300, 500, "DOM 1")
ax.text(-250, -100, 190, "DOM 19")
ax.text(-250, -100, -160, "DOM 40")
ax.text(-650, -300, -500, "DOM 60")

#f3.show()
f3.savefig(I3_SRC+"/phys-services/resources/docs/plots_scalecalculator/I3ScaleCalculator_icecube_3Dview.png", facecolor='white')

