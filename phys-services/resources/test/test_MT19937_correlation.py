#!/usr/bin/env python
import numpy as np
from scipy import stats
from icecube.phys_services import I3MT19937

N=10000

seed_params = [(), (0,),([],),([0],),([0,0],),([0,0,0],)]


for i in range(len(seed_params)):
    for j in range(i,len(seed_params)):
        rng1 = I3MT19937(*seed_params[i])
        rng2 = I3MT19937(*seed_params[j])
        sample1 = [ rng1.uniform(1) for x in range(N) ]
        sample2 = [ rng2.uniform(1) for x in range(N) ]
        r,p = stats.spearmanr(sample1,sample2)
        if i==j:
            assert p==0
        else:
            assert p>0.05
