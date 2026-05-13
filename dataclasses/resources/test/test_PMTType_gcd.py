#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause
#
from pathlib import Path
from os import environ
from icecube.dataclasses import I3Geometry, I3OMGeo
from icecube.dataio import I3File
import numpy as np

'''
This test loops through all the GCDs in the test drive and checks that their PMT type is correct for the OM type for example
a DOM_Standard PMT is not allowed in an mDOM.
'''

pt = I3OMGeo.PMTType
ot = I3OMGeo.OMType

gcddir = Path(environ["I3_TESTDATA"]) / "GCD"
for x in gcddir.iterdir():
    print(f"reading {x}")
    i3file = I3File(str(x))
    while i3file.more():
        frame = i3file.pop_frame()
        if frame.Stop != frame.Geometry:
            continue
        omgeo = I3Geometry.from_frame(frame).omgeo
        for v in omgeo.values():
            if v.omtype in [
                ot.UnknownType, ot.AMANDA, ot.Scintillator, ot.IceAct, ot.WOM, ot.FOM, ot.DMIce, ot.LOM, ot.RadioReceiver,
                ot.POCAM, ot.PencilBeam, ot.RadioEmitter, ot.AcousticEmitter, ot.AbaloneHub, ot.FibreComm,
                ot.AcousticModule, ot.SwedishCamera,
            ]:
                assert v.pmttype in [pt.Unknown]
            elif v.omtype in [ot.IceCube, ot.IceTop, ot.PDOM, ot.isoPDOM]:
                assert v.pmttype in [pt.Unknown, pt.DOM_Standard, pt.DOM_HQE]
            elif v.omtype in [ot.mDOM]:
                assert v.pmttype in [pt.Unknown, pt.mDOM]
            elif v.omtype in [ot.DEgg]:
                assert v.pmttype in [pt.Unknown, pt.DEgg]
            elif v.omtype in [ot.LOM16, ot.LOM18]:
                assert v.pmttype in [pt.Unknown, pt.LOM_Hamamatsu, pt.LOM_NNVT]
            else:
                raise Exception(f"Encountered unknown omtype: {v.omtype}")
