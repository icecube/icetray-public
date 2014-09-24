#!/usr/bin/env python
from icecube import icetray
from icecube.icetray.I3Test import *

ENSURE( icetray.I3Bool(True) == True , "I3Bool == True failed")
ENSURE( icetray.I3Bool(False) == False , "I3Bool == False failed")

ENSURE( icetray.I3Bool(True) != False , "I3Bool != False failed")
ENSURE( icetray.I3Bool(False) != True , "I3Bool != True failed")
