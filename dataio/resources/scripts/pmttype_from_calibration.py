#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import argparse
from itertools import product
from collections import defaultdict
from icecube.icetray import I3Tray, I3Module, I3Frame
from icecube.dataclasses import I3Geometry, I3OMGeo
from icecube.dataio import PMTTypeFromCal


def print_pmtype(frame):
    omgeo = I3Geometry.from_frame(frame).omgeo
    r: defaultdict[tuple[I3OMGeo.OMType, I3OMGeo.PMTType], int] = defaultdict(int)
    for v in omgeo.values():
        r[(v.omtype, v.pmttype)] += 1

    for k in product(I3OMGeo.OMType.values.values(), I3OMGeo.PMTType.values.values()):
        if k not in r:
            continue
        print(f"{k[0].name:>12} {k[1].name:<12} {r[k]}")  # type: ignore[attr-defined]


parser = argparse.ArgumentParser(
    "This script will run the module PMTTypeFromCal on the input file and write the result to the output file.\n"
    + (PMTTypeFromCal.__doc__ or "")
)
parser.add_argument("infile", help="The input file to read from")
parser.add_argument("outfile", help="The output to write to")
args = parser.parse_args()

tray = I3Tray()
tray.Add("I3Reader", FileNameList=[args.infile])
tray.Add(PMTTypeFromCal)
tray.Add(print_pmtype, streams=[I3Frame.Geometry])
tray.Add("I3Writer", filename=args.outfile)
tray.Execute()
