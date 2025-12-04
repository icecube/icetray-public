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
from icecube.icetray import I3Tray, I3Frame, OMKey

GCDD = Path(f'{os.getenv("I3_TESTDATA")}/GCD/')
InGCD = GCDD / 'Off_IC86.2023_data_Run00138821_0105_84_781_GCD.i3.zst'

def AddUpgradeC(frame):
    degg_cals = dataclasses.I3DEggCalMap()
    # add 2 degg pmt cals
    degg_cals[OMKey(87,2,0)] = dataclasses.I3DEggCal()
    degg_cals[OMKey(87,2,1)] = dataclasses.I3DEggCal()
    frame['I3DEggCalibration'] = degg_cals
    # now mDOMs, 24 pmts
    mdom_cals = dataclasses.I3mDOMCalMap()
    for pmt in range(24):
        mdom_cals[OMKey(87,1,pmt)] = dataclasses.I3mDOMCal()
    frame['I3mDOMCalibration'] = mdom_cals
    return True

def AddUpgradeD(frame):
    degg_status = dataclasses.I3DEggStatusMap()
    # add 2 degg pmt status
    degg_status[OMKey(87,2,0)] = dataclasses.I3DEggStatus()
    degg_status[OMKey(87,2,1)] = dataclasses.I3DEggStatus()
    frame['I3DEggStatus'] = degg_status
    # now mDOMs, 24 pmts
    mdom_status = dataclasses.I3mDOMStatusMap()
    for pmt in range(24):
        mdom_status[OMKey(87,1,pmt)] = dataclasses.I3mDOMStatus()
    frame['I3mDOMStatus'] = mdom_status
    return True

def check(frame):
    print('Checking frame:')
    print(frame)
    if frame.Stop == I3Frame.Calibration:
        print('Checking Calibration Frame')
        degg_cals = frame['I3DEggCalibration']
        print(degg_cals)
        assert len(degg_cals) == 2, f'Expected 2 DEgg cals, found {len(degg_cals)}'
        mdom_cals = frame['I3mDOMCalibration']
        print(mdom_cals)
        assert len(mdom_cals) == 24, f'Expected 24 mDOM cals, found {len(mdom_cals)}'
    if frame.Stop == I3Frame.DetectorStatus:
        print('Checking DetectorStatus Frame')
        degg_status = frame['I3DEggStatus']
        print(degg_status)
        assert len(degg_status) == 2, f'Expected 2 DEgg status, found {len(degg_status)}'
        mdom_status = frame['I3mDOMStatus']
        print(mdom_status)
        assert len(mdom_status) == 24, f'Expected 24 mDOM status, found {len(mdom_status)}'

with tempfile.NamedTemporaryFile(suffix='.i3.zst') as tmpi3:
    tray = I3Tray()
    tray.Add('I3Reader', Filename=str(InGCD)),
    tray.Add(AddUpgradeC, 
             streams=[I3Frame.Calibration])
    tray.Add(AddUpgradeD,
             streams=[I3Frame.DetectorStatus])
    tray.Add('Dump')
    tray.AddModule('I3Writer',
                    'writer',
                    filename=tmpi3.name)
    tray.Execute()

    tray = I3Tray()
    tray.Add('I3Reader', Filenamelist=[tmpi3.name])
    tray.Add(check,streams=[I3Frame.DetectorStatus, I3Frame.Calibration]),
    tray.Execute()
