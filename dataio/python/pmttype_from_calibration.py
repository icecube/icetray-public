# SPDX-FileCopyrightText: 2026 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from math import isnan
from icecube.icetray import I3Module, I3Frame
from icecube.dataclasses import I3Geometry, I3Calibration, I3OMGeo
from icecube.icetray.i3logging import log_fatal


class PMTTypeFromCal(I3Module):
    """
    PMTTypeFromCal will set the PMTType in GCD files that were produced before PMTType was added I3OMGeo.
    This operation cannot be performed via serialization because the needed infomration is in the calibration.
    This is only intended to patch up older gcd files, PMTType should be populated through gcdserver for all new GCD files.
    Only OMType IceCube and IceTop will be effected, all other types will reamain pmttype=Unknown.
    This module will log_fatal on anything confusing like anything other than a G frame immeditaly proceeded by a C frame.
    """

    @staticmethod
    def update_pmt_type(geo: I3Geometry, cal: I3Calibration):
        newgeo = I3Geometry(geo)
        for k, v in newgeo.omgeo.items():
            if v.pmttype != I3OMGeo.PMTType.Unknown:
                log_fatal(f"This file already contains PMTTypes other than Unknown: {k}: {v.pmttype}")
            if v.omtype not in [I3OMGeo.OMType.IceCube, I3OMGeo.OMType.IceTop]:
                continue
            c = cal.dom_cal.get(k, None)
            if c is None:
                continue
            if abs(c.relative_dom_eff - 1.35) < 1e-7:
                v.pmttype = I3OMGeo.PMTType.DOM_HQE
            elif abs(c.relative_dom_eff - 1.00) < 1e-7:
                v.pmttype = I3OMGeo.PMTType.DOM_Standard
            elif isnan(c.relative_dom_eff):
                v.pmttype = I3OMGeo.PMTType.Unknown
            else:
                log_fatal(f"{k}: Encountered unknown value for relative_dom_eff: {c.relative_dom_eff}")
        return newgeo

    def __init__(self, context):
        I3Module.__init__(self, context)
        self.geo: I3Frame | None = None

    def Process(self):
        frame = self.PopFrame()
        if frame.Stop == I3Frame.Geometry:
            self.geo = frame
        elif frame.Stop == I3Frame.Calibration:
            if self.geo is not None:
                geo = I3Geometry.from_frame(self.geo)
                cal = I3Calibration.from_frame(frame)
                newgeo = self.update_pmt_type(geo, cal)

                newgeoframe = I3Frame(self.geo)
                del newgeoframe["I3Geometry"]
                newgeoframe["I3Geometry"] = newgeo
                self.PushFrame(newgeoframe)
                self.PushFrame(frame)
                self.geo = None
            else:
                log_fatal(
                    "Entountered calibration frame that wasn't immeditaly proceeded by a geometry frame. "
                    + "This is not a clearly defined case for populating PMTType from calibration frames."
                )
        elif self.geo is None:
            self.PushFrame(frame)
        else:
            log_fatal(
                "Entountered geometry frame that wasn't immeditaly followed by a calibration frame. "
                + "This is not a clearly defined case for populating PMTType from calibration frames."
            )
