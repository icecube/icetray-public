#!/usr/bin/env python3
from icecube.phys_services import I3MT19937

# the docs for the generator specifies the the 10,000th value
# of the generator is 4123659995 for the default constructor
# However, because I3Random.integer has an inclusive upper limit you cannot
# test the exact value and on some compilers it might get rounded down to the 
# next integer
default_mt19937 = I3MT19937()
r = [default_mt19937.integer(2**32-1) for x in range(10000)]
assert(r[-1] in [4123659994, 4123659995])
