#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause

"""A simple test of PairDoubleDouble's properties."""

from icecube.dataclasses import make_pair
from icecube.dataclasses import PairDoubleDouble

from inspect import ismethod

def main():
    """Test PairDoubleDouble's properties."""

    p1 = make_pair(.1, .9)
    p2 = make_pair(.3, .7)

    assert(type(p1) is PairDoubleDouble)
    assert(ismethod(p1.__repr__))
    assert(p1 == make_pair(.1, .9))

    assert(p1 != p2)
    assert(p1 <= p2)

    assert(p1.first < p1.second)
    assert(p1.first < p2.first)
    
    p3 = make_pair(.4, .1)
    assert(p2.first < p3.first)
    assert(p2.second > p3.second)
    assert(p2 < p3)

if __name__ == "__main__":
    main()
