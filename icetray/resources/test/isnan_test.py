#!/usr/bin/env python
from icecube import i3math
from icecube.icetray.I3Test import *

ENSURE( i3math.isnan( float('NaN') ), "isnan( float('NaN') ) should evaluate to True." )
ENSURE( not i3math.isnan( 1.0 ), "isnan( 1.0 ) should evaluate to False." )

