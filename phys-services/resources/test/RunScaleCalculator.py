#!/usr/bin/env python3

import os, sys
import numpy as np

from icecube import icetray
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 
from I3Tray import *

from icecube.phys_services import I3ScaleCalculator
from icecube.icetray import i3logging
from icecube.icetray.I3Test import *

## Let's grab a fairly ordinary IC86 GCD file, and some events
TESTDATA = os.path.expandvars("$I3_TESTDATA")
gcdfile = TESTDATA+"/GCD/GeoCalibDetectorStatus_2020.Run134142.Pass2_V0.i3.gz"
eventfile = TESTDATA+"/level3-filter-muon/Level2_IC86.2016_NuMu.021002.muonL3test.i3.zst"

## Set the logging? (If you want!)
i3logging.set_level_for_unit('I3ScaleCalculator', 'DEBUG')

tray = I3Tray()

tray.AddModule("I3Reader","reader",FileNameList = [gcdfile, eventfile])

## Invoke the scale calculators
class TryTheScaleCalculators(icetray.I3ConditionalModule):
  """
  Grab the I3Geometry from the "G", and use it to make some plots!
  """
  def __init__(self, ctx):
        icetray.I3ConditionalModule.__init__(self, ctx)
        self.AddParameter("TrackName", "Whose track to compute Containment/Scale?", "PoleMuonLlhFit")
        self.AddOutBox("OutBox")
         
  def Configure(self):
    self.trackname = self.GetParameter("TrackName")
  
  def Geometry(self, frame):
    geo = frame['I3Geometry']
      
    ## Create an I3ScaleCalculator
    self.calculator1 = I3ScaleCalculator(geo, I3ScaleCalculator.IC86_STRICT, I3ScaleCalculator.IT81_STRICT)
    ## Let's also try a Deepcore one, and an Empty one (defaule is to Guess) -- Empty should spit an error and output NAN
    self.calculator2 = I3ScaleCalculator(geo, I3ScaleCalculator.DEEPCORE_BELOWDUST, I3ScaleCalculator.IT_EMPTY)

 
  def Physics(self, frame):
    eh = frame["I3EventHeader"]
    ev = eh.event_id
    
    ## Let's compute one, for a track, just to make sure it works
    if frame.Has(self.trackname):
      tr = frame[self.trackname]
      scale_icetop = self.calculator1.scale_icetop(tr)
      scale_inice = self.calculator1.scale_inice(tr)
      i3logging.log_info("--- Event %d ----"%ev)
      i3logging.log_info("IceTop (area) scale = %f"%scale_icetop)
      i3logging.log_info("InIce (volume) scale = %f"%scale_inice)
      scale_infill = self.calculator2.scale_icetop(tr)
      scale_deepcore = self.calculator2.scale_inice(tr)
      i3logging.log_info("IceTop-Infill (area) scale = %f"%scale_infill)
      i3logging.log_info("InIce-Deepcore (volume) scale = %f"%scale_deepcore)

      ## Check them!
      if ev==12:
        ENSURE_DISTANCE(scale_icetop, 5.88168, 0.00001, "First event: mismatched IceTop scale")
        ENSURE_DISTANCE(scale_inice, 0.392260, 0.00001, "First event: mismatched InIce scale")
        ENSURE(np.isnan(scale_infill), "First event: IceTop-Infill scale should be NAN")
        ENSURE_DISTANCE(scale_deepcore, 2.727131, 0.00001, "First event: mismatched DeepCore scale")
      elif ev==13:
        ENSURE_DISTANCE(scale_icetop, 27.207993, 0.00001, "Second event: mismatched IceTop scale")
        ENSURE_DISTANCE(scale_inice, 0.484603, 0.00001, "Second event: mismatched InIce scale")
        ENSURE(np.isnan(scale_infill), "Second event: IceTop-Infill scale should be NAN")
        ENSURE_DISTANCE(scale_deepcore, 2.332320, 0.00001, "Second event: mismatched DeepCore scale")
      elif ev==24:
        ENSURE_DISTANCE(scale_icetop, 2.815122, 0.00001, "Third event: mismatched IceTop scale")
        ENSURE_DISTANCE(scale_inice, 0.441217, 0.00001, "Third event: mismatched InIce scale")
        ENSURE(np.isnan(scale_infill), "Third event: IceTop-Infill scale should be NAN")
        ENSURE_DISTANCE(scale_deepcore, 1.849429, 0.00001, "Third event: mismatched DeepCore scale")
      elif ev==27:
        ENSURE_DISTANCE(scale_icetop, 24.131528, 0.00001, "Fourth event: mismatched IceTop scale")
        ENSURE_DISTANCE(scale_inice, 0.513447, 0.00001, "Fourth event: mismatched InIce scale")
        ENSURE(np.isnan(scale_infill), "Fourth event: IceTop-Infill scale should be NAN")
        ENSURE_DISTANCE(scale_deepcore, 2.495409, 0.00001, "Fourth event: mismatched DeepCore scale")
      else:
        i3logging.log_fatal("An event number that I wasn't expecting showed up: %d"%ev)
      

tray.Add(TryTheScaleCalculators)

tray.Execute(20)  ## This will give you the first four (non-NullSplit) P-frames.

