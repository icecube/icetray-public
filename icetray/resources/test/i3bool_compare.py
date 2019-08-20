#!/usr/bin/env python
from icecube import icetray
from icecube.icetray.I3Test import *


ENSURE( icetray.I3Bool(True) == True , "I3Bool == True failed")
ENSURE( icetray.I3Bool(False) == False , "I3Bool == False failed")

ENSURE( icetray.I3Bool(True) != False , "I3Bool != False failed")
ENSURE( icetray.I3Bool(False) != True , "I3Bool != True failed")

ENSURE( bool(icetray.I3Bool(True))  == True , "I3Bool == True failed")
ENSURE( bool(icetray.I3Bool(False)) == False, "I3Bool == False failed")

b = repr(icetray.I3Bool(True))
ENSURE( 'true' in b.lower(), "I3Bool does not repr true")
b = repr(icetray.I3Bool(False))
ENSURE( 'false' in b.lower(), "I3Bool does not repr false")

b = icetray.I3Bool(True).__str__()
ENSURE( 'true' in b.lower(), "I3Bool does not print true")
b = icetray.I3Bool(False).__str__()
ENSURE( 'false' in b.lower(), "I3Bool does not print false")
