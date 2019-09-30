#!/usr/bin/env python
from icecube import icetray
from icecube.icetray.I3Test import *

ENSURE( icetray.I3Int(12) == icetray.I3Int(12) , "I3Int == I3Int failed" )
ENSURE( icetray.I3Int(12) != icetray.I3Int(13) , "I3Int != I3Int failed" )
ENSURE( icetray.I3Int(12) < icetray.I3Int(13) , "I3Int < I3Int failed" )
ENSURE( icetray.I3Int(12) <= icetray.I3Int(13) , "I3Int <= I3Int failed" )
ENSURE( icetray.I3Int(14) > icetray.I3Int(13) , "I3Int > I3Int failed" )
ENSURE( icetray.I3Int(14) >= icetray.I3Int(13) , "I3Int >= I3Int failed" )

ENSURE( icetray.I3Int(12) == 12 , "I3Int == int failed" )
ENSURE( icetray.I3Int(12) != 13 , "I3Int != int failed" )
ENSURE( icetray.I3Int(12) < 13 , "I3Int < int failed" )
ENSURE( icetray.I3Int(12) <= 13 , "I3Int <= int failed" )
ENSURE( icetray.I3Int(14) > 13 , "I3Int > int failed" )
ENSURE( icetray.I3Int(14) >= 13 , "I3Int >= int failed" )

ENSURE( icetray.I3Int(12) == 12. , "I3Int == float failed" )
ENSURE( icetray.I3Int(12) != 13. , "I3Int != float failed" )
ENSURE( icetray.I3Int(12) < 13. , "I3Int < float failed" )
ENSURE( icetray.I3Int(12) <= 13. , "I3Int <= float failed" )
ENSURE( icetray.I3Int(14) > 13. , "I3Int > float failed" )
ENSURE( icetray.I3Int(14) >= 13. , "I3Int >= float failed" )

ENSURE( icetray.I3Int(0) == False, "I3Int == False failed" )
ENSURE( icetray.I3Int(0) != True, "I3Int != True failed" )
ENSURE( icetray.I3Int(1) == True, "I3Int == True failed" )
ENSURE( icetray.I3Int(1) != False, "I3Int != False failed" )

ENSURE( bool(icetray.I3Int(0)) == False, "I3Int == False failed" )
ENSURE( bool(icetray.I3Int(1)) == True,  "I3Int == True failed" )
