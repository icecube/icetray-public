# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

class Detector:
  """Base class for individual detector types
     the implementations know how to get information
     out of the frame, default values, etc"""

  def __init__(self):
    self.name = "DetectorNameUnset"
    self.shapes = ['o', 's', 'P', "X", "D", "*"]
    self.positions = {}
    self.measuredData = {}
    self.shouldDraw = True   #Decides if this should be drawn
    self.colorMapType = 'gist_rainbow'

class PulseData:
    __slots__ = ("t",
                 "t_plane",
                 "charge",
                 "hlc")

    def __init__(self, t, charge, hlc):
        self.t = t
        self.charge = charge
        self.hlc = hlc
