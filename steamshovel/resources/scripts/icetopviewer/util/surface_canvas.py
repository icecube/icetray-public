import numpy as np
import matplotlib
from matplotlib import pyplot
from matplotlib.widgets import MultiCursor, CheckButtons

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

        ##These are the colors of the particles/cores/directions in the array subplot
        self.colors = ['k', 'r', 'b', 'g']

        self.fig = pyplot.figure(figsize=(18, 12), constrained_layout=True)
        gs = self.fig.add_gridspec(8, 10)
        self.axlist = {}

        # Text information about each particle, run id, etc
        # Location Top Left
        self.axlist["info"] = self.fig.add_subplot(gs[:4, :2])
        ax = self.axlist["info"]
        self.__reset_textbox()
        ax.text(0.05, 0.95, "No Q/P Frames found yet", ha="left", va="top", color='k', transform=ax.transAxes)


        # Shows Check Bottons in a box where it is possible to set visible the following parameters
        # Location Bottom of the infobox
        self.axlist["checkboxes"] = self.fig.add_subplot(gs[3:4,:2])
        ax = self.axlist["checkboxes"]
        labels = [detector.GetKeyName() for detector in self.detectors] + self.particleKeys
        activated = [True for i in range(len(labels))]
        self.check = CheckButtons(ax, labels, activated)
        self.check.on_clicked(self.get_visible)

        #The layout of the array and the hit detectors
        # Location Top between the infobox and the ldf
        self.axlist["array"] = self.fig.add_subplot(gs[:4, 2:5])
        self.__reset_array()

        #Lateral distribution
        # Location Top Right
        self.axlist["ldf"] = self.fig.add_subplot(gs[:4, 5:10])
        ax = self.axlist["ldf"]
        self.__reset_ldf()

        #Time delay w.r.t plane
        # Location Bottom right
        self.axlist["time"] = self.fig.add_subplot(gs[4:, 5:], sharex=self.axlist["ldf"])
        self.__reset_timedelay()

        # Radio waveforms
        # Location Bottom Left
        self.axlist["waveforms"] = self.fig.add_subplot(gs[4:, :5])
        ax = self.axlist["waveforms"]

        # Shows a cursor for the ldf and time plot since the 2 plots share the same x-axis
        self.multi = MultiCursor(self.fig.canvas, (self.axlist["ldf"], self.axlist["time"]),
                                 color='r', lw=1, horizOn=False, vertOn=True)


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

    ###########################
    ##  Reset the various plots
    ###########################

    # The reset is done before the new DAQ or P frame is analyzed
    def __reset_array(self):
        ax = self.axlist["array"]
        ax.clear()

        ax.set_aspect("equal")
        ax.set_xlim(-600, 600)
        ax.set_ylim(-600, 600)
        ax.set_xlabel("x / m")
        ax.set_ylabel("y / m", labelpad=-10)

    def __reset_ldf(self):
        ax = self.axlist["ldf"]
        ax.clear()

        ax.set_ylim(0.1, 1e4)
        ax.set_yscale("log")
        ax.set_ylabel("S / VEM")
        ax.set_xlim(0, 10)
        ax.autoscale(True, axis='x')
        ax.set_xscale("linear")

    def __reset_timedelay(self):
        ax = self.axlist["time"]
        ax.clear()
        ax.axhline(0, color='k', linestyle='--', alpha=0.3)
        ax.set_xlabel("Axial Radius / m")
        ax.set_ylabel("Time w.r.t Plane Front / ns")
        ax.set_xlim(0, 10)
        ax.autoscale(True, axis='x')

    def __reset_textbox(self):
        ax = self.axlist["info"]
        ax.clear()
        ax.set_xticks([])
        ax.set_yticks([])


    ######################
    ##  Main frame parsers
    ######################

    # Here the geometry for each detector is stored as a dict.
    # The position of each detector is stored in a numpy array with a key = detector key
    def update_geometry_frame(self, frame):
        self.__reset_array()

        for detector in self.detectors:
            detector.ExtractFromGFrame(frame)
            detector.DrawGeometry(self.axlist["array"])

    # Here all the needed info from DAQ or P frame are stored. Then the plots are drawn.
    def update_DAQ_or_P_frame(self, frame):

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
        for idet, detector in enumerate(self.detectors):
            detector.ExtractFromQPFrame(frame)
            detector.DrawLDF(self.axlist["ldf"], self.particles[0])
            detector.DrawGeometry(self.axlist["array"])
            detector.DrawShowerFront(self.axlist["time"], self.particles[0])
        self.axlist["ldf"].legend(loc='upper right', prop={'size': 8})

        self.__draw_core()
        self.__reset_textbox()
        self.__fill_text_box(frame)

        # colormap for time delay label next to the geometry plot
        self.fig.colorbar(matplotlib.cm.ScalarMappable(cmap=self.detectors[0].colorMapType),
                          ax=self.axlist["array"], aspect=40, label='Normalized Time')

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
