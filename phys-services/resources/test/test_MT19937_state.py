#!/usr/bin/env python
import numpy as np
from scipy import stats
from icecube.phys_services import I3MT19937

N=10000

rng1 = I3MT19937([])

for z in range(100):
    state = rng1.state
    rng2 =I3MT19937()
    rng2.state = state

    sample1 =  [ rng1.uniform(1) for x in range(N) ]
    sample2 =  [ rng2.uniform(1) for x in range(N) ]

    assert sample1==sample2

