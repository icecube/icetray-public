#!/usr/bin/env python3
from icecube import icetray
from icecube.icetray.I3Test import ENSURE


# we're testing for _equality_, not _identity_, so we need to hide it from linters
ENSURE( icetray.I3Bool(True) == True , "I3Bool == True failed")          # noqa: E712
ENSURE( icetray.I3Bool(False) == False , "I3Bool == False failed")       # noqa: E712

ENSURE( icetray.I3Bool(True) != False , "I3Bool != False failed")        # noqa: E712
ENSURE( icetray.I3Bool(False) != True , "I3Bool != True failed")         # noqa: E712

ENSURE( bool(icetray.I3Bool(True))  == True , "I3Bool == True failed")   # noqa: E712
ENSURE( bool(icetray.I3Bool(False)) == False, "I3Bool == False failed")  # noqa: E712

# test for _identity_
ENSURE( bool(icetray.I3Bool(True))  is True , "I3Bool == True failed")
ENSURE( bool(icetray.I3Bool(False)) is False, "I3Bool == False failed")

b = repr(icetray.I3Bool(True))
ENSURE( 'true' in b.lower(), "I3Bool does not repr true")
b = repr(icetray.I3Bool(False))
ENSURE( 'false' in b.lower(), "I3Bool does not repr false")

b = icetray.I3Bool(True).__str__()
ENSURE( 'true' in b.lower(), "I3Bool does not print true")
b = icetray.I3Bool(False).__str__()
ENSURE( 'false' in b.lower(), "I3Bool does not print false")
