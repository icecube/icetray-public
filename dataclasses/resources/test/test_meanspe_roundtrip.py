#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
from pathlib import Path
from itertools import product
import tempfile
import math
from icecube import dataio # noqa: F401
from icecube import dataclasses
from icecube.icetray import I3Tray, I3Frame
from icecube.frame_object_diff.segments import compress, uncompress

GCDD = Path(f'{os.getenv("I3_TESTDATA")}/GCD/')
GCDS = [GCDD / 'GeoCalibDetectorStatus_2020.Run134142.Pass2_V0.i3.gz',
        GCDD / 'GeoCalibDetectorStatus_2020.Run134142.Pass1_V0.i3.gz',
        GCDD / 'Off_IC86.2023_data_Run00138821_0105_84_781_GCD.i3.zst',]


def check(frame):
    cal = frame['I3Calibration']
    global mean_spes
    if mean_spes is None:
        mean_spes = [dataclasses.mean_spe_charge(cal.dom_cal[_]) for _ in cal.dom_cal]
    else:
        orig_spes = [dataclasses.mean_spe_charge(cal.dom_cal[_]) for _ in cal.dom_cal]
        assert all([_a == _b for _a, _b in zip(mean_spes, orig_spes)])


def reference(frame):
    cal = frame['I3Calibration']
    assert math.isclose(dataclasses.mean_spe_charge(cal.dom_cal[list(cal.dom_cal.keys())[100]]),
                        0.8212159498951772)


with tempfile.NamedTemporaryFile(suffix='.i3.zst') as tmpi3:
    for gcd_test, gcd_base in product(GCDS, GCDS):
        gcd_test = str(gcd_test)
        gcd_base = str(gcd_base)
        mean_spes = None
        tray = I3Tray()
        tray.Add('I3Reader', Filename=gcd_test)
        tray.Add(compress,
                 base_filename=gcd_base)
        tray.AddModule('I3Writer',
                       'writer',
                       filename=tmpi3.name,
                       streams=[I3Frame.Calibration])
        tray.Execute()

        tray = I3Tray()
        tray.Add('I3Reader', Filenamelist=[tmpi3.name, gcd_test])
        tray.Add(uncompress,
                 base_filename=gcd_base)
        tray.Add(check,
                 streams=[I3Frame.Calibration])
        if gcd_test == str(GCDS[0]):
            tray.Add(reference,
                     streams=[I3Frame.Calibration])
        tray.Execute()
