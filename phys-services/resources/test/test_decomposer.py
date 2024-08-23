#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import numpy as np
from collections import defaultdict
from pathlib import Path
from icecube import icetray, dataio, phys_services
from icecube.icetray import I3Tray

TESTGCD = Path(os.path.expandvars('$I3_TESTDATA')) / 'GCD'
RTOL = 0.4 # string 21 seems to have larger variance in module positions


def ztol(s):
    mdz = 13.
    if s > 86:
        mdz = 2.9
    elif s >= 79:
        mdz = 6.4

    return mdz


class DecomposerTestModule(icetray.I3Module):
    ''' This module simply tests that the geometry decomposer works properly
    '''
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)


    def Geometry(self, frame) :
        omgeo = frame['I3OMGeoMap']
        orgeo = frame['I3Geometry'].omgeo

        odict = defaultdict(set)
        for k in orgeo.keys():
            assert omgeo[k].position == orgeo[k].position
            assert omgeo[k].orientation == orgeo[k].orientation
            assert omgeo[k].omtype == orgeo[k].omtype
            odict[k.string].add(k.om)

        modgeo = frame['I3ModuleGeoMap']
        udict = defaultdict(list)
        sdict = defaultdict(list)
        for k in modgeo.keys():
            if modgeo[k].module_type in [0, 30, 45]:
                udict[k.string].append(k.om)
                continue
            sdict[k.string].append(list(modgeo[k].pos))

        for s in sdict:

            assert len(odict[s]) == len(udict[s]) + len(sdict[s])
            pss = np.asarray(sdict[s])
            if s > 0:
                assert len(pss) >= 60

            assert np.all(np.ediff1d(pss[::-1, 2]) > ztol(s))

            xme = pss[:, 0].mean()
            yme = pss[:, 1].mean()
            rad = (pss[:, 0] - xme)**2 + (pss[:, 1] - yme)**2

            assert np.all(rad < RTOL**2)

        self.PushFrame(frame)


tray = I3Tray()
tray.Add('I3Reader',
         Filenamelist=[str(_) for _ in TESTGCD.glob('*.i3.*')])
tray.Add('Delete',
         keys=['I3ModuleGeoMap', 'I3ExtraModuleGeoMap', 'I3OMGeoMap', 'I3StationGeoMap', 'StartTime', 'Subdetectors', 'EndTime', 'BedrockZ', 'DepthAtZ0'])
tray.Add('I3GeometryDecomposer')
tray.Add(DecomposerTestModule)
tray.Execute()
