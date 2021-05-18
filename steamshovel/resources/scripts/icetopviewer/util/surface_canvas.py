import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.widgets import MultiCursor, CheckButtons, RadioButtons

from util.GeometryTools import ProjectToObslev

from icecube.dataclasses import I3Constants
from icecube import dataclasses
from icecube.icetray import I3Units

class SurfaceCanvas():
    """
       This is the canvas that gets updated at each frame
       It holds the list of particles that need to be plotted and the various detector
       types that need to extract and plot data from the frame.
       Note that the first particle in the list will be used for the LDF and the others
       are just there for the core location information.
       Each detector class is in charge of knowing what to do with the information
       from the frame and what it should be plotting on the various subfigures of the canvas
    """

    def __init__(self, detectors, particleKeys):
        ''' Init with a list of particle frame keys and a list of instances of the detectors '''

        self.detectors = detectors
        self.particleKeys = particleKeys
        self.particleKeys_inframe = []
        self.frame = None

        ##These are the colors of the particles/cores/directions in the array subplot
        self.colors = ['k', 'r', 'b', 'g']

        self.fig = plt.figure(figsize=(18, 12), constrained_layout=True)
        gs = self.fig.add_gridspec(8, 20)
        self.axlist = {}

        # Text information about each particle, run id, etc
        # Location Top Left
        self.axlist["info"] = self.fig.add_subplot(gs[:5, :4])
        ax = self.axlist["info"]
        self.__reset_textbox(ax)
        ax.text(0.05, 0.95, "No Q/P Frames found yet", ha="left", va="top", color='k', transform=ax.transAxes)

        # Shows Check Buttons in a box where it is possible to set visible the following parameters (labels)
        # Location Bottom of the infobox
        self.axlist["checkboxes"] = self.fig.add_subplot(gs[3:5, :4])
        ax = self.axlist["checkboxes"]

        #The layout of the array and the hit detectors
        # Location Top between the infobox and the ldf
        self.axlist["array"] = self.fig.add_subplot(gs[:4, 4:9])
        self.__reset_array()

        self.axlist["colorbar"] = self.fig.add_subplot(gs[:4, 9:10])
        self.__reset_colorbar()

        #Lateral distribution
        # Location Top Right
        self.axlist["ldf"] = self.fig.add_subplot(gs[:4, 10:])
        ax = self.axlist["ldf"]
        self.__reset_ldf()

        #Time delay w.r.t plane
        # Location Bottom right
        self.axlist["time"] = self.fig.add_subplot(gs[4:, 10:], sharex=self.axlist["ldf"])
        self.__reset_timedelay()

        # Radio waveforms
        # Location Bottom Left
        self.axlist["info_radio"] = self.fig.add_subplot(gs[5:, :4])
        ax = self.axlist["info_radio"]
        self.__reset_textbox(ax)
        ax.text(0.05, 0.95, "No Antenna selected yet", ha="left", va="top", color='k', transform=ax.transAxes)

        # Shows Radio Buttons in a box where it is possible to set visible the following parameters (labels)
        # Location Bottom of the infobox_radio
        self.axlist["radio_buttons"] = self.fig.add_subplot(gs[6:, :4])
        ax = self.axlist["radio_buttons"]
        self.__reset_textbox(ax)

        self.axlist["waveforms_time"] = self.fig.add_subplot(gs[4:6, 4:10])
        ax = self.axlist["waveforms_time"]
        self.axlist["waveforms_freq"] = self.fig.add_subplot(gs[6:, 4:10])
        ax = self.axlist["waveforms_freq"]



        # Shows a cursor for the ldf and time plot since the 2 plots share the same x-axis
        self.multi = MultiCursor(self.fig.canvas, (self.axlist["ldf"], self.axlist["time"]),
                                 color='r', lw=1, horizOn=False, vertOn=True)

    def CheckBoxFunction(self, frame, ax):
        self.__reset_textbox(ax)
        labels = [detector.GetKeyName() for detector in self.detectors] + [el for el in self.particleKeys if el in frame]
        activated = [True for i in range(len(labels))]
        self.check = CheckButtons(ax, labels, activated)
        self.check.on_clicked(self.get_visible)
        return

    def get_visible(self, label):
        for detector in self.detectors:
            if label == detector.GetKeyName():
                detector.ToggleHidden()
        if label in self.particleKeys:
            if label in self.core:
                self.core[label].set_visible(not self.core[label].get_visible())
            if label in self.arrow:
                self.arrow[label].set_visible(not self.arrow[label].get_visible())
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()
        return

    def RadioFunction(self, label):
        self.__reset_waveforms()
        self.detectors[2].antennakeys = [str(label)]
        self.detectors[2].DrawAntennasPlots(self.frame, self.axlist)
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()
        return

    def RadioVisible(self, frame):
        ax = self.axlist["radio_buttons"]
        self.__reset_textbox(ax)
        labels = [el for el in self.detectors[2].antennakeys if el in frame]
        self.radio = RadioButtons(ax, labels)
        self.radio.on_clicked(self.RadioFunction)

    ###########################
    ##  Reset the various plots
    ###########################

    # The reset is done before the new DAQ or P frame is analyzed
    def __reset_array(self):
        ax = self.axlist["array"]
        ax.clear()

        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        ax.set_aspect("equal")
        ax.set_xlim(-600, 600)
        ax.set_ylim(-600, 600)
        ax.set_xlabel("x / m")
        ax.set_ylabel("y / m")

    def __reset_colorbar(self):
        ax = self.axlist["colorbar"]
        ax.clear()
        ax.set_xticks([])
        ax.set_yticks([])
        col_map = plt.get_cmap('gist_rainbow')
        # mpl.colorbar(ax, cmap=col_map, orientation='vertical', label='Normalized Time', aspect=40)
        mpl.colorbar.ColorbarBase(ax, cmap=col_map, orientation='vertical', label='Normalized Time')

    def __reset_ldf(self):
        ax = self.axlist["ldf"]
        ax.clear()

        # ax.set_ylim(1e-02, 1e4)
        ax.set_yscale("log")
        ax.set_ylabel("S / VEM")
        ax.set_xlim(0, 10)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        ax.autoscale(True, axis='x')
        ax.set_xscale("linear")

    def __reset_timedelay(self):
        ax = self.axlist["time"]
        ax.clear()
        ax.axhline(0, color='k', linestyle='--', alpha=0.3)
        ax.set_xlabel("Axial Radius / m")
        ax.set_ylabel("Time w.r.t Plane Front / ns")
        ax.set_xlim(0, 10)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        ax.autoscale(True, axis='x')

    def __reset_textbox(self, ax):
        ax.clear()
        ax.set_xticks([])
        ax.set_yticks([])

    def __reset_waveforms(self):
        ax = self.axlist["waveforms_time"]
        ax.clear()
        ax = self.axlist["waveforms_freq"]
        ax.clear()


    ######################
    ##  Main frame parsers
    ######################

    # Here the geometry for each detector is stored as a dict.
    # The position of each detector is stored in a numpy array with a key = detector key
    def update_geometry_frame(self, frame):
        self.__reset_array()
        self.frame = frame
        self.CheckBoxFunction(frame,self.axlist["checkboxes"])
        for detector in self.detectors:
            detector.ExtractFromGFrame(frame)
            detector.DrawGeometry(self.axlist["array"])

    # Here all the needed info from DAQ or P frame are stored. Then the plots are drawn.
    def update_DAQ_or_P_frame(self, frame):
        self.frame = frame
        self.CheckBoxFunction(frame,self.axlist["checkboxes"])

        if not len(self.particleKeys):
            print("WARNING: You did not define a particle yet!")
            return

        self.particles = []
        for name in self.particleKeys:
            if name in frame.keys():
                self.particles.append(frame[name])
                self.particleKeys_inframe.append(name)

        self.__reset_ldf()
        self.__reset_array()
        self.__reset_timedelay()
        self.__reset_waveforms()

        for idet, detector in enumerate(self.detectors):
            detector.ExtractFromQPFrame(frame)
            detector.DrawLDF(self.axlist["ldf"], self.particles[0])
            detector.DrawGeometry(self.axlist["array"])
            detector.DrawShowerFront(self.axlist["time"], self.particles[0])
        self.axlist["ldf"].legend(loc='upper right', prop={'size': 8})

        # Labels for the antennas must get separately
        self.detectors[2].antennakeys = self.detectors[2].GetDefaultAntennaKeys()
        self.RadioVisible(frame)

        self.__draw_core()
        self.__reset_textbox(self.axlist["info"])
        self.__fill_text_box(frame)

        # colormap for time delay label next to the geometry plot
        # if not self.colorBar:
        #     self.colorBar = self.fig.colorbar(matplotlib.cm.ScalarMappable(cmap=self.detectors[0].colorMapType),
        #                           ax=self.axlist["colormap"], aspect=40, label='Normalized Time')

    def ArrayOnClick(self, event):
        # Check if the click is in the correct location
        if event.inaxes == self.axlist["array"].axes:
            # Get the position of the closest antenna
            click_pos = np.asarray([event.xdata, event.ydata])
            self.__reset_waveforms()
            self.detectors[2].AntennaOnClick(click_pos, self.frame, self.axlist)
            self.fig.canvas.draw()
            self.fig.canvas.flush_events()



    #################################
    ##  Detector non-specific drawing
    #################################
    def __draw_core(self):
        ax = self.axlist["array"]
        self.core = {}
        self.arrow = {}
        for ipart, particle in enumerate(self.particles):
            core = dataclasses.I3Position(particle.pos)
            core = ProjectToObslev(core, particle.dir)

            self.core[self.particleKeys_inframe[ipart]] = \
                ax.scatter(core.x, core.y, color=self.colors[ipart % len(self.colors)])

            xy = np.array([particle.dir.x, particle.dir.y])
            xy = xy / np.sqrt(sum(xy**2)) * 100

            self.arrow[self.particleKeys_inframe[ipart]] = \
                ax.arrow(core.x, core.y, xy[0], xy[1], head_width=10, alpha=0.7, color=self.colors[ipart % len(self.colors)])

    def __fill_text_box(self, frame):
        ax = self.axlist["info"]

        #First draw the meta-info about the run
        words = ""
        
        if "I3EventHeader" in frame:
            header = frame["I3EventHeader"]
            evtID = header.event_id
            runID = header.run_id

            #Fill the left column
            words += "Run ID:   {}\n".format(runID)
            words += "Event ID: {}\n".format(evtID)
            words += "\n"

        #Print column 1
        ax.text(0.05, 0.95, words, ha="left", va="top",
                    color="k", transform=ax.transAxes)

        #Iterate over the particles and draw their info
        nCols = 2
        nRows = 2
        item = 0
        for name in self.particleKeys:
            if name in frame.keys():
                words = ""

                particle = frame[name]
                words += "---{}---\n".format(name)
                words += "Zen: {0:0.1f} deg\n".format(particle.dir.zenith/I3Units.degree)
                words += "Azi: {0:0.1f} deg\n".format(particle.dir.azimuth/I3Units.degree)
                words += "lg(E/eV): {0:0.2f}\n".format(np.log10(particle.energy/I3Units.eV))
                words += "\n"

                icol = item %2
                irow = int(item/2)

                ax.text(0.05 + 0.5*icol, 0.82 - 0.25*irow, words, ha="left", va="top",
                    color=self.colors[item % len(self.colors)], transform=ax.transAxes)

                item += 1
