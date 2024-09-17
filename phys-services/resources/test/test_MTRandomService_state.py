#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import numpy as np
from scipy import stats
from icecube.phys_services import I3MTRandomService

N=10000

rng1 = I3MTRandomService([])

for z in range(100):
    state = rng1.state
    rng2 =I3MTRandomService()
    rng2.state = state

    sample1 =  [ rng1.uniform(1) for x in range(N) ]
    sample2 =  [ rng2.uniform(1) for x in range(N) ]

    assert sample1==sample2

