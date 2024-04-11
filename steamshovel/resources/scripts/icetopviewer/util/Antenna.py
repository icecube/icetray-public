# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from .Detector import Detector, PulseData
from .GeometryTools import get_radius

import numpy as np

from icecube import icetray
from icecube import dataclasses
from icecube.dataclasses import FFTData3D
from icecube.icetray import *
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants
from icecube import radcube
from icecube import taxi_reader

from matplotlib.collections import PatchCollection
import matplotlib.patches as mpatches
import matplotlib.path as mpath

"""
TODO list:
1. The overall antenna plots need to be double checked.
2. The isADC function is SUPER slow. We should check if it can be optimized
3. LDF and time plots
"""

class Antenna(Detector):
    """docstring for Antenna"""

    def __init__(self):
        super().__init__()
        self.antennakeys = self.GetDefaultAntennaKeys()
        self.antennas_pulse_patches = PatchCollection([])
        self.color = "b"
        self.name = "Antenna"
        self.timeUnit = I3Units.nanosecond
        self.timeUnitName = "ns"
        self.freqUnit = I3Units.megahertz
        self.freqUnitName = "MHz"
        self.voltageUnit = I3Units.volt * 1e-6
        self.voltageUnitName = r"$\mu$V"
        self.specUnit = self.voltageUnit / self.freqUnit
        self.specUnitName = self.voltageUnitName + "/" + self.freqUnitName
        self.antenna_lables = []
        self.AntennaStationID = "None"
        self.isADC = False

    def GetDefaultAntennaKeys(self):
        return [taxi_reader.taxi_tools.taxi_antenna_frame_name(), "ResampledVoltageMap", radcube.GetDefaultSimEFieldName()]

    def GetKeyName(self):
        return self.name

    def ExtractFromGFrame(self, frame):
        assert (frame.Stop == icetray.I3Frame.Geometry)

        self.positions.clear()

        # Check all things in geometry frame I3Geometry
        for key in frame.keys():
            if key == "I3GeometryDiff": continue
            i3geometry = frame[key]
            for antkey, ant in i3geometry.antennageo:
                pos = ant.position
                self.positions[antkey] = np.asarray((pos.x, pos.y, pos.z))

    def DrawGeometry(self, ax):
        antenna_patches = []
        for pos in self.positions.values():
            antenna_patches.append(self.antennaPatches(pos[:2]))
        self.antennas_position_patches = PatchCollection(antenna_patches, match_original=True)
        ax.add_collection(self.antennas_position_patches)

    def ExtractFromQPFrame(self, frame):
        self.measuredData.clear()

        for framekey in self.antennakeys:
            if framekey in frame.keys() and len(frame[framekey]) != 0:
                ant_map = frame[framekey]
                self.measuredData[framekey] = ant_map

    def DrawLDF(self, ax, particle):
        pass

    def DrawShowerFront(self, ax, particle):
        pass

    def GetDrawOptions(self, frame):
        print("Current pulse keys are", self.antennakeys)
        user_response = input("Enter desired keys: ")
        partKeys = user_response.split()

        self.antennakeys = []
        for key in partKeys:
            if key != "":
                self.antennakeys.append(key)

        print(self.name, "keys set to", self.antennakeys)

    def GetAntennaLabels(self, frame):
        antenna_lables = []
        for key in self.antennakeys:
            if key in frame.keys():
                for antkey in frame[key].keys():
                    if [antkey.GetAntennaID(), antkey.GetStationID()] not in antenna_lables:
                        antenna_lables.append([antkey.GetAntennaID(), antkey.GetStationID()])
        self.antenna_lables = antenna_lables

    def AntennaOnClick(self, click_pos, frame, axlist):
        #Finds the antenna closest to the point that is clicked
        #and uses it to update the plots

        if self.positions == {}: return
        antennaKeys = []
        antennaPos = []
        for antKey in self.positions.keys():
            antennaKeys.append(antKey)
        for pos in self.positions.values():
            antennaPos.append(pos[:2])
        antennaPos = np.asarray(antennaPos)
        antennaKeys = np.asarray(antennaKeys)
        distance = np.sum((antennaPos - click_pos) ** 2, axis=1)
        _min = np.argmin(distance)
        self.AntennaStationID = antennaKeys[_min]
        self.__fill_text_box(frame, axlist["info_radio"])
        self.DrawAntennasPlots(frame, axlist)

    def DrawAntennasPlots(self, frame, axlist):
        if self.AntennaStationID == "None":
            return

        for key in self.antennakeys:
            if key in frame.keys():
                if self.AntennaStationID not in frame[key].keys():
                    return
                # TODO: I did not find a more efficient way to do it #Federico
                if isinstance(frame[key], dataclasses.EFieldTimeSeriesMap):
                    self.TimeEfieldPlot(frame[key], axlist, 1, key)
                elif isinstance(frame[key], dataclasses.I3AntennaDataMap):
                    self.TimeFreqDbPlot(frame[key], axlist, 1, key)
                else:
                    log_fatal("Key: ({}) is type ({}). I don't know what this is!".format(key, type(frame[key])))
        return

    def TimeFreqDbPlot(self, antDataMap, axlist, plotFrac, plotLabel):
        (times, hilbert1, signal1, freqs, f_signal1) = self.AntDataMapToPython(antDataMap, 0)
        (times, hilbert2, signal2, freqs, f_signal2) = self.AntDataMapToPython(antDataMap, 1)

        ax = axlist["waveforms_time"]

        self.MakeTimePlot(ax, times, signal1, plotFrac, 'b', plotLabel + " Ch.1")
        self.MakeTimePlot(ax, times, signal2, plotFrac, 'r', plotLabel + " Ch.2")
        if not self.isADC:
            self.MakeHilbertPlot(ax, times, hilbert1, plotFrac, 'k', plotLabel + " Ch.1")
            self.MakeHilbertPlot(ax, times, hilbert2, plotFrac, 'g', plotLabel + " Ch.2")
        ax.legend(prop={'size': 5})

        ax = axlist["waveforms_freq"]

        if self.isADC:
            self.MakeFreqPlot(ax, freqs, f_signal1, plotFrac, 'b', plotLabel + "Ch.1")
            self.MakeFreqPlot(ax, freqs, f_signal2, plotFrac, 'r', plotLabel + "Ch.2")
        else:
            self.MakedBmHzPlot(ax, freqs, f_signal1, plotFrac, 'b', plotLabel + " Ch.1")
            self.MakedBmHzPlot(ax, freqs, f_signal2, plotFrac, 'r', plotLabel + " Ch.2")

        ax.legend(prop={'size': 5})

    def TimeEfieldPlot(self, vectorMap, axlist, plotFrac, plotLabel):
        #Makes a plot of the Efield times series for the passed in values

        times, tsX, tsY, tsZ, freqs, specX, specY, specZ = self.I3RadVector3DToPython(vectorMap)

        ax = axlist["waveforms_time"]

        self.MakeTimePlot(ax, times, tsX, plotFrac, "b", plotLabel + " Grid W")
        self.MakeTimePlot(ax, times, tsY, plotFrac, "r", plotLabel + " Grid N")
        self.MakeTimePlot(ax, times, tsZ, plotFrac, "k", plotLabel + " Vertical")
        ax.legend(prop={'size': 5})

        ax = axlist["waveforms_freq"]

        self.MakeFreqPlot(ax, freqs, specX, plotFrac, 'b', plotLabel + " Grid W")
        self.MakeFreqPlot(ax, freqs, specY, plotFrac, 'r', plotLabel + " Grid N")
        self.MakeFreqPlot(ax, freqs, specZ, plotFrac, 'k', plotLabel + " Vertical")
        ax.legend(prop={'size': 5})

    def I3RadVector3DToPython(self, vectorMap):
        #Converts the waveform data into numpy arrays
        #The times and freqs (x-axis values) are converted into the requested units here

        if self.AntennaStationID not in vectorMap.keys():
            logging.log_warn("Antenna key ({}) not found in the map!".format(self.AntennaStationID))
            return 0, 0, 0, 0, 0, 0, 0, 0,

        fftData = dataclasses.FFTData3D(vectorMap[self.AntennaStationID])
        times, tsX, tsY, tsZ = radcube.RadTraceToPythonList(fftData.GetTimeSeries())
        freqs, specX, specY, specZ = radcube.RadTraceToPythonList(fftData.GetFrequencySpectrum())

        specX = np.abs(specX)
        specY = np.abs(specY)
        specZ = np.abs(specZ)

        times /= self.timeUnit
        freqs /= self.freqUnit

        return times, tsX, tsY, tsZ, freqs, specX, specY, specZ

    def AntDataMapToPython(self, antDataMap, channel_no):
        #Converts the waveform data into numpy arrays
        #The times and freqs (x-axis values) are converted into the requested units here

        #Make sure that this antenna key is in the map
        if self.AntennaStationID not in antDataMap.keys():
            logging.log_warn("Antenna key ({}) not found in the map!".format(self.AntennaStationID))
            return [], [], [], [], []

        channelMap = antDataMap[self.AntennaStationID]

        #Make sure that this channel key is in the map
        if channel_no not in channelMap.keys():
            logging.log_warn("Channel key ({}) not found in the map of antenna key ({})!".format(channel_no,self.AntennaStationID))
            return [], [], [], [], []

        fft = channelMap[channel_no].GetFFTData()
        times, timeseriespy = radcube.RadTraceToPythonList(fft.GetTimeSeries())
        times, hilbertpy = radcube.RadTraceToPythonList(dataclasses.fft.GetHilbertEnvelope(fft))
        freqs, spectrumpy = radcube.RadTraceToPythonList(fft.GetFrequencySpectrum())

        spectrumpy = np.abs(spectrumpy)
        times /= self.timeUnit
        freqs /= self.freqUnit

        return times, hilbertpy, timeseriespy, freqs, spectrumpy


    def MakeTimePlot(self, ax, times, amps, plotFrac, plotColor, plotLabel):
        #Plots the time series from the passed in values

        plotBins = int(len(times) * plotFrac) - 1

        if not self.isADC:
            amps = np.array(amps)
            amps /= self.voltageUnit

        if self.isADC:
            mean = np.mean(amps[0:plotBins])
            ax.plot(times[0:plotBins], amps[0:plotBins]-mean, color=plotColor, label=plotLabel)
        else:
            rms = np.std(amps[0:plotBins])
            ax.plot(times[0:plotBins], amps[0:plotBins], color=plotColor,
                    label=plotLabel + " - RMS: {0:0.3f}".format(rms))

        if not self.isADC:
            thisMax = max(amps[0:plotBins])
            thisMin = min(amps[0:plotBins])
            ylow, yhigh = ax.get_ylim()
            ymax = max([ylow, yhigh, thisMax, -thisMin])
            ax.set_ylim(-ymax * 1.02, ymax * 1.02)
            ax.set_ylabel("Amplitude [" + self.voltageUnitName + "]")
        else:
            ax.set_ylabel("Amplitude [ADC]")

        ax.set_xlabel("Time [" + self.timeUnitName + "]")
        ax.set_xlim(times[0], times[plotBins])

    def MakeHilbertPlot(self, ax, times, amps, plotFrac, plotColor, plotLabel):
        #Draws the Hilbert envelope from the passed in values

        plotBins = int(len(times) * plotFrac) - 1

        if not self.isADC:
            amps = np.array(amps)
            amps /= self.voltageUnit

        ax.plot(times[0:plotBins], amps[0:plotBins], color=plotColor, label=plotLabel, linestyle='dashed')

        if not self.isADC:
            ax.set_ylabel("Amplitude [" + self.voltageUnitName + "]")
        else:
            ax.set_ylabel("Amplitude [ADC]")

        ax.set_xlabel("Time [" + self.timeUnitName + "]")
        ax.set_xlim(times[0], times[plotBins])

    def MakeFreqPlot(self, ax, freqs, amps, plotFrac, plotColor, plotLabel):
        #Plots the frequency spectrum from the passed in values
        #Automatically converts to logscale if the values are large

        plotBins = int(len(freqs) * plotFrac) - 1

        if not self.isADC:
            amps = np.array(amps)
            amps /= self.specUnit

        ax.plot(freqs[1:plotBins], amps[1:plotBins], color=plotColor, label=plotLabel)
        ylow, yhigh = ax.get_ylim()
        if (yhigh >= 1000):
            ax.set_yscale("log")
        ax.set_xlabel("Frequency [" + self.freqUnitName + "]")
        ax.set_xlim(freqs[0], freqs[plotBins])

        if not self.isADC:
            ax.set_ylabel("Amplitude [" + self.specUnitName + "]")
        else:
            ax.set_ylabel("Amplitude [ADC/Hz]")

    def MakedBmHzPlot(self, ax, freqs, amps, plotFrac, plotColor, plotLabel):
        #Plots the frequency spectrum in dBm/Hz for the passed in amplitudes

        plotBins = int(len(freqs) * plotFrac) - 1

        amps = np.array(amps)
        df = (freqs[1] - freqs[0]) * self.freqUnit

        # Convert to power
        pz = (amps * np.sqrt(2.) * df) ** 2
        power = pz / (50 * I3Units.ohm)
        # Power per 1mW
        powerPermW = power / (1.e-3 * I3Units.joule / I3Units.second)
        # Ensure no zeros
        powerPermW += 1e-100 * min([i for i in powerPermW if i > 0])
        # dBmHz
        dBmHz = 10 * np.log10(powerPermW / df * I3Units.hertz)

        low = 1
        high = len(dBmHz)
        for ibin in range(1, len(dBmHz) - 3):
            if (0.5 * (dBmHz[ibin + 1] + dBmHz[ibin + 2]) - 0.5 * (dBmHz[ibin] + dBmHz[ibin - 1]) > 30 and
                    dBmHz[ibin + 1] > -300):
                low = ibin + 1
            if (0.5 * (dBmHz[ibin + 1] + dBmHz[ibin + 2]) - 0.5 * (dBmHz[ibin] + dBmHz[ibin - 1]) < -30 and
                    dBmHz[ibin] > -300):
                high = ibin

        plotFreqs = freqs[low:high + 1]
        plotAmps = dBmHz[low:high + 1]

        ax.plot(plotFreqs, plotAmps, color=plotColor, label=plotLabel)

        ylow, yhigh = ax.get_ylim()

        ax.set_xlabel("Frequency [" + self.freqUnitName + "]")
        ax.set_xlim(freqs[0], freqs[plotBins])

        ax.set_ylabel("Amplitude [dBm Hz]")

    def ToggleHidden(self):
        self.shouldDraw = not self.shouldDraw
        self.antennas_position_patches.set_visible(self.shouldDraw)
        self.antennas_pulse_patches.set_visible(self.shouldDraw)

    def antennaPatches(self, pos, x=10., y=10., rotation=0.):
        # Draws an antenna patch like a greek cross with and internal square of side length off_set * 2
        Path = mpath.Path
        off_set = 2.0
        path_data = [
            (Path.MOVETO, [off_set, off_set]),
            (Path.LINETO, [off_set, off_set+y/2]),
            (Path.LINETO, [-off_set, off_set+y/2]),
            (Path.LINETO, [-off_set, off_set]),
            (Path.LINETO, [-off_set-x/2, off_set]),
            (Path.LINETO, [-off_set-x/2, -off_set]),
            (Path.LINETO, [-off_set, -off_set]),
            (Path.LINETO, [-off_set, -off_set-y/2]),
            (Path.LINETO, [off_set, -off_set-y/2]),
            (Path.LINETO, [off_set, -off_set]),
            (Path.LINETO, [off_set+x/2, -off_set]),
            (Path.LINETO, [off_set+x/2, off_set]),
            (Path.CLOSEPOLY, [off_set, off_set])]
        codes, verts = zip(*path_data)
        path = mpath.Path(verts + pos, codes)
        patch = mpatches.PathPatch(path, edgecolor="None", facecolor=self.color, alpha=1.0)
        return patch

    def __fill_text_box(self, frame, ax):
        #Writes the currently selected antenna into the text box
        self.__reset_textbox(ax)
        words = ""
        # Fill the info
        words += "Antenna Station ID:\n" \
                 "{}\n".format(self.AntennaStationID)
        ax.text(0.05, 0.95, words, ha="left", va="top",
                color="k", transform=ax.transAxes)

    def __reset_textbox(self, ax):
        ax.clear()
        ax.set_xticks([])
        ax.set_yticks([])
