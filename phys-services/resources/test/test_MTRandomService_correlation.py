#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import numpy as np
from scipy import stats
from icecube.phys_services import I3MTRandomService

N=10000

seed_params = [(), (0,),([],),([0],),([0,0],),([0,0,0],)]


for i in range(len(seed_params)):
    for j in range(i,len(seed_params)):
        rng1 = I3MTRandomService(*seed_params[i])
        rng2 = I3MTRandomService(*seed_params[j])
        sample1 = [ rng1.uniform(1) for x in range(N) ]
        sample2 = [ rng2.uniform(1) for x in range(N) ]
        r,p = stats.spearmanr(sample1,sample2)
        if i==j:
            assert p==0
        else:
            assert p>0.05
