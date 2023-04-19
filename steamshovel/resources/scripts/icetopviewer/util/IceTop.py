from .Detector import Detector, PulseData

from .GeometryTools import get_radius

import numpy as np

from icecube import icetray
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants

from matplotlib.collections import PatchCollection
from matplotlib.patches import Circle
from matplotlib import colors, cm

from icecube.recclasses import I3LaputopParams
from icecube.recclasses import LaputopLDF
from icecube.recclasses import LaputopParameter
from icecube.recclasses import LaputopFrontDelay
from icecube.recclasses import LaputopEnergy

class IceTop(Detector):
    """docstring for IceTop"""
    def __init__(self):
        super(IceTop, self).__init__()
        self.pulsekeys = self.GetDefaultPulseKeys()
        self.color = "k"
        self.name = "IceTop"
        self.minPatchSize = 5
        self.maxPatchSize = self.minPatchSize * 5
        self.time_delay = []
        self.tanks_position_patches = PatchCollection([])
        self.tanks_pulse_patches = PatchCollection([])
    
    def GetDefaultPulseKeys(self):
        return ['OfflineIceTopHLCTankPulses', "HLCTankPulses"]

    def GetKeyName(self):
        return self.name

    def ExtractFromGFrame(self, frame):
        assert(frame.Stop == icetray.I3Frame.Geometry)

        self.positions.clear()

        # Check all things in geometry frame I3Geometry
        for key in frame.keys():
            if key == "I3GeometryDiff": continue
            i3geometry = frame[key]
            for stnkey, station in i3geometry.stationgeo:
                pos_sta = np.zeros(3)
                for tank in station:
                    pos_sta[0] += tank.position.x
                    pos_sta[1] += tank.position.y
                    pos_sta[2] += tank.position.z
                pos_sta /= len(station)
                for tank in station:
                    for omkey in tank.omkey_list:
                        pos = tank.position
                        self.positions[str(omkey)] = np.asarray((pos.x, pos.y, pos.z))


    def DrawGeometry(self, ax):
        if not self.shouldDraw: return

        tanks_patches = []
        for pos in self.positions.values():
            tanks_patches.append(Circle(pos, self.minPatchSize, edgecolor="None", facecolor=self.color, alpha=0.5))
        self.tanks_position_patches = PatchCollection(tanks_patches, match_original=True)
        ax.add_collection(self.tanks_position_patches)

        amps = []
        positions = []
        time = []
        pulses_patches = []

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]
            for omkey in pulses.keys():
                pulse = pulses[omkey]
                pos = self.positions[str(omkey)]
                totalCharge = sum([p.charge for p in pulse])

                positions.append(pos)
                amps.append(totalCharge)
                time.append(pulse[0].t)

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
            pulses_patches.append(Circle(pos, size, edgecolor="None",  facecolor=t, alpha=0.2))
        self.tanks_pulse_patches = PatchCollection(pulses_patches, match_original=True)
        ax.add_collection(self.tanks_pulse_patches)


    def ExtractFromQPFrame(self, frame):
        self.measuredData.clear()
        self.laputopParams = None

        for framekey in self.pulsekeys:
            if framekey in frame.keys() and len(frame[framekey]):
                # gets Tank Pulses and stores them in a dict with the detector key for the unique geometry match
                recopulse_map = frame[framekey]
                pulses = {}
                for omkey in recopulse_map.keys():
                    pulses_per_tank = []
                    for pulse in recopulse_map[omkey]:
                        pulses_per_tank.append(PulseData(pulse.time,
                                                         pulse.charge,
                                                         True))
                    pulses[omkey] = pulses_per_tank
                self.measuredData[framekey] = pulses

        if "LaputopParams" in frame.keys():
            self.laputopParams = I3LaputopParams.from_frame(frame, "LaputopParams")


    def __DrawLaputopLDF(self, ax, radii):
        lg_s125 = self.laputopParams.value(LaputopParameter.Log10_S125)
        lg_s125_err = self.laputopParams.error(LaputopParameter.Log10_S125)
        s125 = 10 ** lg_s125
        beta = self.laputopParams.value(LaputopParameter.Beta)
        beta_err = self.laputopParams.error(LaputopParameter.Beta)

        rmin = np.amin(radii, axis=0)
        rmax = np.amax(radii, axis=0)
        rspace = 10 ** np.linspace(np.log10(rmin*0.9), np.log10(rmax*1.1), 50)
        ldfVals = np.vectorize(self.laputopParams.expected_signal)(rspace)
        ldfErr = np.vectorize(self.laputopParams.expected_signal_error)(rspace)
        curveVals = np.vectorize(self.laputopParams.expected_shower_front_delay)(rspace)
        curveErr = np.vectorize(self.laputopParams.expected_shower_front_delay_error)(rspace)

        ax.plot(rspace, ldfVals, color=self.color)
        ax.fill_between(rspace, ldfVals+ldfErr, ldfVals-ldfErr, color=self.color, alpha=0.2)
        ax.scatter(125, s125, marker='X', color=self.color)


    def DrawLDF(self, ax, particle):
        if not self.shouldDraw:
            return

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]

            radii = []
            amps = []
            time = []

            for omkey in pulses.keys():
                pulse = pulses[omkey]
                pos = self.positions[str(omkey)]
                r = get_radius(particle, pos)

                radii.append(r)
                amps.append(sum([p.charge for p in pulse]))
                time.append(pulse[0].t)

            cmap = cm.get_cmap(self.colorMapType)
            time = np.subtract(time, min(time))
            time = np.divide(time, max(time))
            time = cmap(time)
            ax.scatter(radii,  amps, c=time, alpha=0.4, marker=self.shapes[ikey%len(self.shapes)], label=framekey)

        if self.laputopParams:
            self.__DrawLaputopLDF(ax, radii)

        # Silent stations
        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]
            radii = []
            for scintkey in [el for el in self.positions.keys() if el not in pulses.keys()]:
                pos = self.positions[scintkey]
                r = get_radius(particle, pos)
                radii.append(r)
            amps = [0.01 for i in range(len(radii))]
            ax.scatter(radii, amps, c="w", alpha=0.4, marker=self.shapes[ikey % len(self.shapes)], edgecolors="k")


    def DrawShowerFront(self, ax, particle):
        if not self.shouldDraw: return

        for ikey, framekey in enumerate(self.measuredData.keys()):
            pulses = self.measuredData[framekey]

            radii = []
            amps = []
            time = []

            tCore = particle.time
            core = np.array([particle.pos[i] for i in range(3)])
            nDir = np.array([particle.dir.x, particle.dir.y, particle.dir.y])

            for omkey in pulses.keys():
                pulse = pulses[omkey]
                pos = self.positions[str(omkey)]
                r = get_radius(particle, pos)
                relPos = pos - core
                delay = tCore + (nDir[0]*relPos[0]+nDir[1]*relPos[1]-nDir[2]*relPos[2]) / I3Constants.c
                delay = (delay - pulse[0].t) / I3Units.ns

                radii.append(r)
                amps.append(delay)
                time.append(pulse[0].t)

            # Same as above
            cmap = cm.get_cmap(self.colorMapType)
            time = np.subtract(time, min(time))
            time = np.divide(time, max(time))
            time = cmap(time)
            ax.scatter(radii, amps, c=time, alpha=0.4, marker=self.shapes[ikey%len(self.shapes)])

        if self.laputopParams:
            self.__DrawLaputopTiming(ax, radii)

        # Silent stations are not needed for the time plot


    def __DrawLaputopTiming(self, ax, radii):
        rmin = np.amin(radii, axis=0)
        rmax = np.amax(radii, axis=0)
        rspace = 10 ** np.linspace(np.log10(rmin*0.9), np.log10(rmax*1.1), 50)
        curveVals = np.vectorize(self.laputopParams.expected_shower_front_delay)(rspace) / I3Units.ns
        curveErr = np.vectorize(self.laputopParams.expected_shower_front_delay_error)(rspace) / I3Units.ns

        ax.plot(rspace, curveVals, color=self.color)
        ax.fill_between(rspace, curveVals+curveErr, curveVals-curveErr, color=self.color, alpha=0.2)


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
        self.tanks_position_patches.set_visible(self.shouldDraw)
        self.tanks_pulse_patches.set_visible(self.shouldDraw)
