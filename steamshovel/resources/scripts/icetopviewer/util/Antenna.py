from .Detector import Detector, PulseData

from .GeometryTools import get_radius

import numpy as np

from icecube import icetray
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants

from matplotlib.patches import RegularPolygon
from matplotlib.collections import PatchCollection
"""
This class needs to be done.
In order to be completed the radio functions are needed.
Thus, this class will be completed when those functions will be implemented
"""
class Antenna(Detector):
    """docstring for Antenna"""
    def __init__(self):
        super(Antenna, self).__init__()
        self.pulsekeys = self.GetDefaultPulseKeys()
        self.color = "b"
        self.name = "Antennas"
        self.antenna_position_patches = PatchCollection([])
    
    def GetDefaultPulseKeys(self):
        return ['None']

    def GetKeyName(self):
        return self.name

    def ExtractFromGFrame(self, frame):
        assert(frame.Stop == icetray.I3Frame.Geometry)

        self.positions.clear()

        #Check all things in geometry frame I3Geometry
        for key in frame.keys():
            i3geometry = frame[key]
            for antkey, ant in i3geometry.antennageo:
                pos = ant.position
                self.positions[str(antkey)] = np.asarray((pos.x, pos.y, pos.z))

    def DrawGeometry(self, ax):
        if not self.shouldDraw:
            return

        antennas_patches =[]
        for pos in self.positions.values():
            antennas_patches.append(RegularPolygon(pos[:2], 3, radius=7, edgecolor="None", facecolor=self.color, alpha=1.0))
        self.antenna_position_patches = PatchCollection(antennas_patches, match_original=True)
        ax.add_collection(self.antenna_position_patches)


    def ExtractFromQPFrame(self, frame):
        del self.measuredData
        self.measuredData = {}

        for framekey in self.pulsekeys:
            if framekey in frame.keys():
                ant_map = frame[framekey]



    def DrawLDF(self, ax, particle):
        if not self.shouldDraw:
            return

    def DrawShowerFront(self, ax, particle):
        if not self.shouldDraw:
            return


    def GetDrawOptions(self, frame):
        print("Current pulse keys are", self.pulsekeys)
        user_response = input("Enter desired keys: ")
        partKeys = user_response.split()

        self.pulsekeys = []
        for key in partKeys:
            if key != "":
                self.pulsekeys.append(key)

        print(self.name, "keys set to", self.pulsekeys)

    def ToggleHidden(self):
        self.shouldDraw = not self.shouldDraw
        self.antenna_position_patches.set_visible(self.shouldDraw)