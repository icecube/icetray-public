#!/usr/bin/env python
from icecube import dataclasses
from icecube.icetray.I3Test import *

ENSURE( dataclasses.I3Double(12.0) == dataclasses.I3Double(12.0) , "I3Double == I3Double failed" )
ENSURE( dataclasses.I3Double(12.0) != dataclasses.I3Double(13) , "I3Double != I3Double failed" )
ENSURE( dataclasses.I3Double(12.0) < dataclasses.I3Double(13) , "I3Double < I3Double failed" )
ENSURE( dataclasses.I3Double(12.0) <= dataclasses.I3Double(13) , "I3Double <= I3Double failed" )
ENSURE( dataclasses.I3Double(14.0) > dataclasses.I3Double(13) , "I3Double > I3Double failed" )
ENSURE( dataclasses.I3Double(14.0) >= dataclasses.I3Double(13) , "I3Double >= I3Double failed" )

ENSURE( dataclasses.I3Double(12.0) == 12 , "I3Double == int failed" )
ENSURE( dataclasses.I3Double(12.0) != 13 , "I3Double != int failed" )
ENSURE( dataclasses.I3Double(12.0) < 13 , "I3Double < int failed" )
ENSURE( dataclasses.I3Double(12.0) <= 13 , "I3Double <= int failed" )
ENSURE( dataclasses.I3Double(14.0) > 13 , "I3Double > int failed" )
ENSURE( dataclasses.I3Double(14.0) >= 13 , "I3Double >= int failed" )

ENSURE( dataclasses.I3Double(12.0) == 12. , "I3Double == float failed" )
ENSURE( dataclasses.I3Double(12.0) != 13. , "I3Double != float failed" )
ENSURE( dataclasses.I3Double(12.0) < 13. , "I3Double < float failed" )
ENSURE( dataclasses.I3Double(12.0) <= 13. , "I3Double <= float failed" )
ENSURE( dataclasses.I3Double(14.0) > 13. , "I3Double > float failed" )
ENSURE( dataclasses.I3Double(14.0) >= 13. , "I3Double >= float failed" )

ENSURE( dataclasses.I3Double(0.0) == False , "I3Double == False failed" )
ENSURE( dataclasses.I3Double(0.0) != True , "I3Double != True failed" )
ENSURE( dataclasses.I3Double(1.0) == True , "I3Double == True failed" )
ENSURE( dataclasses.I3Double(1.0) != False , "I3Double != False failed" )

ENSURE(bool(dataclasses.I3Double(0.0))==False, "bool(0) should be false")
ENSURE(bool(dataclasses.I3Double(1.0))==True, "bool(0) should be false")
