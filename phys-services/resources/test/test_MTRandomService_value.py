#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.phys_services import I3MTRandomService, I3GSLRandomService

# the docs for the generator specifies the the 10,000th value
# of the generator is 4123659995 for the default constructor
# However, because I3Random.integer has an inclusive upper limit you cannot
# test the exact value and on some compilers it might get rounded down to the
# next integer
rand0 = I3MTRandomService()
r0 = [rand0.integer(0xFFFF_FFFF) for _ in range(10000)]
assert r0[-1] in [4123659994, 4123659995]

rand1 = I3MTRandomService()
r1 = [rand1.integer32() for _ in range(10000)]
assert r1[-1] == 4123659995

rand2 = I3MTRandomService(5489)
r2 = [rand2.integer(0xFFFF_FFFF) for _ in range(10000)]
assert r2[-1] in [4123659994, 4123659995]

rand3 = I3MTRandomService(5489)
r3 = [rand3.integer32() for _ in range(10000)]
assert r3[-1] == 4123659995

# GSL random is the same algorithm so lets check that as well
rand4 = I3GSLRandomService(5489)
r4 = [rand4.integer(0xFFFF_FFFF) for _ in range(10000)]
assert r4[-1] in [4123659994, 4123659995]

rand4 = I3GSLRandomService(5489)
r4 = [rand4.integer32() for _ in range(10000)]
assert r4[-1] == 4123659995
