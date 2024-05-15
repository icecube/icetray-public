# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""A demonstration of a converter for a complex object written in pure Python"""

from icecube.dataclasses import I3FlasherInfo, I3FlasherInfoVect
from icecube import tableio
import numpy as n

class I3FlasherInfoVectConverter(tableio.I3Converter):
    booked = I3FlasherInfoVect

    def CreateDescription(self,fivect: I3FlasherInfoVect):
        desc = tableio.I3TableRowDescription()
        desc.is_multi_row = True
        desc.add_field("string",tableio.types.Int32, "","string number of the flashing DOM")
        desc.add_field("om",tableio.types.UInt32, "","OM number of the flashing DOM")
        desc.add_field("flash_time",tableio.types.Float64, "ns","Time (in nsec) in 25 nsec units, of the LED flash time.")
        desc.add_field("mask",tableio.types.UInt32, "bitmask","indicates which LEDs were flashing")
        desc.add_field("width",tableio.types.Int32, "","flasher width")
        desc.add_field("rate",tableio.types.Int32, "","flasher rate")
        desc.add_field("brightness",tableio.types.Int32, "","brightness level (1..127)")

        desc.add_field("atwd_bin_size",tableio.types.Float64, "ns","bin width of the ATWD3")
        desc.add_field("raw_atwd3",tableio.types.Int64, "counts","ATWD waveform of the LED current pulse",128)

        return desc

    def FillRows(self,fivect: I3FlasherInfoVect,rows: tableio.I3TableRow):

        start = rows.current_row
        for i,flasherinfo in enumerate(fivect):
            rows.current_row = start + i
            rows["string"]     = flasherinfo.flashing_om.om
            rows["om"]         = flasherinfo.flashing_om.string
            rows["flash_time"] = flasherinfo.flash_time
            rows["mask"]       = flasherinfo.mask
            rows["width"]      = flasherinfo.width
            rows["rate"]       = flasherinfo.rate
            rows["brightness"] = flasherinfo.led_brightness
            rows["atwd_bin_size"] = flasherinfo.atwd_bin_size
            #rows["pedestal_subtracted_atwd"] = ?
            rows["raw_atwd3"] = n.asarray(flasherinfo.raw_atwd3)

        return rows.current_row + 1 - start

    def GetNumberOfRows(self, fivect):
        return len(fivect)

#tableio.I3ConverterRegistry.register(I3FlasherInfoVectConverter)
