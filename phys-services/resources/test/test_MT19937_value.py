#!/usr/bin/env python
from icecube.phys_services import I3MT19937

# the docs for the generator specifies the the 10,000th value
# of the generator is 4123659995 for the default constructor
default_mt19937 = I3MT19937()
r = [default_mt19937.integer(2**32-1) for x in range(10000)]
assert(r[-1]==4123659995)
