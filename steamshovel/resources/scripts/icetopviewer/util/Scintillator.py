from .Detector import Detector, PulseData

from .GeometryTools import get_radius

import numpy as np

from icecube import icetray
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants

from matplotlib.collections import PatchCollection
from matplotlib.patches import Rectangle
from matplotlib import colors, cm

class Scintillator(Detector):
    """docstring for Scintillator"""
    def __init__(self):
        super(Scintillator, self).__init__()
        self.pulsekeys = self.GetDefaultPulseKeys()
        self.color = "r"
        self.name = 'Scintillators'
    
        self.minPatchSize = 10
        self.maxPatchSize = self.minPatchSize * 5

        self.scint_pulse_patches = PatchCollection([])
        self.scint_position_patches = PatchCollection([])

    def GetDefaultPulseKeys(self):
        return ['ScintRecoPulses', 'ScintRecoPulsesHighGain', 'ScintRecoPulsesMediumGain', 'ScintRecoPulsesLowGain', "SiPMRecoPulses"]

    def GetKeyName(self):
        return self.name

    def ExtractFromGFrame(self, frame):
        assert(frame.Stop == icetray.I3Frame.Geometry)

        self.positions.clear()

        #Check all things in geometry frame I3Geometry
        for key in frame.keys():
            if key == "I3GeometryDiff": continue
            i3geometry = frame[key]
            for scintkey, scint in i3geometry.scintgeo:
                pos = scint.position
                self.positions[(scintkey.station, scintkey.panel)] = np.asarray((pos.x, pos.y, pos.z))


    def DrawGeometry(self, ax):
        if not self.shouldDraw: return

        scint_patches = []
        for pos in self.positions.values():
            scint_patches.append(Rectangle(pos, self.minPatchSize, self.minPatchSize, edgecolor="None", facecolor=self.color, alpha=1.0))
        self.scint_position_patches = PatchCollection(scint_patches, match_original=True)
        ax.add_collection(self.scint_position_patches)

        amps = []
        positions = []
        time = []
        pulses_patches = []

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]
            for scintkey in pulses.keys():
                pulse = pulses[scintkey]
                pos = self.positions[(scintkey.station, scintkey.panel)]

                positions.append(pos)
                amps.append(pulse.charge)
                time.append(pulse.t)

        if not len(amps): return

        amps = np.log10(amps)
        minAmp = min(amps)
        maxAmp = max(amps)

        relPatchSize = self.minPatchSize*1.2 + (self.maxPatchSize - self.minPatchSize*1.2) * (amps - minAmp) / (maxAmp - minAmp + 0.01)

        # The color map is used for  showing the time delay of the pulses.
        # The time is set to 0 by subtracting the min and then it is normalized by dividing the max
        cmap = cm.get_cmap(self.colorMapType)
        time = np.subtract(time, min(time))
        time = np.divide(time, max(time))
        time = cmap(time)
        for size, pos, t in zip(relPatchSize, positions, time):
            pulses_patches.append(Rectangle(pos, size/2., size/2., edgecolor="None", facecolor=t, alpha=0.2))
        self.scint_pulse_patches = PatchCollection(pulses_patches, match_original=True)
        ax.add_collection(self.scint_pulse_patches)

    def ExtractFromQPFrame(self, frame):
        self.measuredData.clear()

        for framekey in self.pulsekeys:
            if framekey in frame.keys() and len(frame[framekey]) != 0:
                # handle scintillators
                scint_map = frame[framekey]
                pulses = {}

                for scint, vector in scint_map:
                    pulses[scint] = PulseData(vector[0].time, vector[0].charge, False)

                self.measuredData[framekey] = pulses

    def DrawLDF(self, ax, particle):
        if not self.shouldDraw:
            return

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]

            radii = []
            amps = []
            time = []

            for scintkey in pulses.keys():
                pulse = pulses[scintkey]
                pos = self.positions[(scintkey.station, scintkey.panel)]
                r = get_radius(particle, pos)

                radii.append(r)
                amps.append(pulse.charge)
                time.append(pulse.t)

            cmap = cm.get_cmap(self.colorMapType)
            time = np.subtract(time, min(time))
            time = np.divide(time, max(time))
            time = cmap(time)
            ax.scatter(radii,  amps, c=time, alpha=0.4, marker=self.shapes[(ikey+1)%len(self.shapes)], label=framekey)

            # Silent stations
            radii = []
            for key in [el for el in self.positions.keys() if el not in pulses.keys()]:
                pos = self.positions[key]
                r = get_radius(particle, pos)
                radii.append(r)
            amps = [0.01 for i in range(len(radii))]
            ax.scatter(radii,  amps, c="w", alpha=0.4, marker=self.shapes[(ikey+1)%len(self.shapes)], edgecolors="k")


    def DrawShowerFront(self, ax, particle):
        if not self.shouldDraw:
            return

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]

            radii = []
            amps = []
            time = []

            tCore = particle.time
            core = np.array([particle.pos[i] for i in range(3)])
            nDir = np.array([particle.dir.x, particle.dir.y, particle.dir.y])

            for scintkey in pulses.keys():
                pulse = pulses[scintkey]
                pos = self.positions[(scintkey.station, scintkey.panel)]
                r = get_radius(particle, pos)
                relPos = pos - core
                delay = tCore + (nDir[0]*relPos[0]+nDir[1]*relPos[1]-nDir[2]*relPos[2]) / I3Constants.c
                delay = (delay - pulse.t) / I3Units.ns

                radii.append(r)
                amps.append(delay)
                time.append(pulse.t)

            # Same as above
            cmap = cm.get_cmap(self.colorMapType)
            time = np.subtract(time, min(time))
            time = np.divide(time, max(time))
            time = cmap(time)
            ax.scatter(radii,  amps, c=time, alpha=0.4, marker=self.shapes[(ikey+1)%len(self.shapes)])

            # Silent stations are not needed for the time plot

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
        self.scint_position_patches.set_visible(self.shouldDraw)
        self.scint_pulse_patches.set_visible(self.shouldDraw)

