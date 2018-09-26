import numpy as np

from matplotlib import pyplot
from matplotlib.font_manager import FontProperties
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.collections import PatchCollection, LineCollection
from matplotlib.patches import Wedge, FancyArrowPatch, Circle, Ellipse, Rectangle, Polygon
from matplotlib.lines import Line2D
from matplotlib.transforms import Bbox
from matplotlib.artist import setp as msetp
from matplotlib.text import Text
from matplotlib.axes import Axes

from icecube import dataclasses, icetray
from icecube.steamshovel.artists.util import to_shower_cs, signed_r_z, mpl_hist, rainbow_text
I3Constants = dataclasses.I3Constants
I3Units = icetray.I3Units
# optional: Read *Params objects needed to draw LDF
# and list many reconstructed values
try: from icecube.recclasses import I3LaputopParams, \
    LaputopParameter, LaputopLDF, LaputopFrontDelay, LaputopEnergy
except: pass

class TankData(object):
    __slots__ = ("type", "ctr", "phi", "pos", "label", 
                 "h_snow", "h_ice", "pulses", "doms", "r", "z")

class ScintData(object):
    __slots__ = ("pos", "label", "pulses", "doms", "r", "z")

class PulseData(object):
    __slots__ = ("t", "t_plane", "charge", "hlc")

    def __init__(self, t, charge, hlc):
        self.t = t
        self.charge = charge
        self.hlc = hlc

def set_sizes(collection, sizes):
    'Compatibility fix for matplotlib < 1.3'
    if hasattr(collection, 'set_sizes'):
        collection.set_sizes(sizes)
    else:
        collection._sizes = sizes

# make plots look the same on every system
import matplotlib
matplotlib.rcdefaults()
for g in ("xtick", "ytick"):
    matplotlib.rc(g, direction="out")
matplotlib.rc("axes", labelsize="large", titlesize="large")

# some global constants
calibration_key = "I3Calibration"
detectorstatus_key = "I3DetectorStatus"
eventheader_key = "I3EventHeader"
fadc_tbin = 25.0 # nanosecond, from IceTop detector paper
atwd_tbin = 3.33 # nanosecond, from IceTop detector paper

# remember discovered scintillator OMKeys if any
class cache:
    geometry_time_range = None
    scintillator_omkeys = []

class IceTopCanvas(FigureCanvas):

    class tank: pass
    class scint: pass

    def __init__(self, settings):
        fig = pyplot.figure()
        FigureCanvas.__init__(self, fig)
        # We create plot objects in advance and fill with data later.
        # This is signficantly faster than clearing and recreating the
        # collections over and over. We need to stick to this if we 
        # want to keep a reasonable reaction time.
        fig.patch.set_color("0.8")
        self.tmin = 0
        self.tmax = 1
        self.ax_trace = None
        self.xlim = {}
        self.ylim = {}

        ### top left: top view on array
        ax = self.ax_array = fig.add_axes((0.07, 0.10, 0.43, 0.80),
                                          autoscale_on=False)
        self.shower_plane_intersection = Line2D([], [],
            linestyle="--", color="k", alpha=0.6, zorder=0)
        ax.add_line(self.shower_plane_intersection)

        self.array_detectors = PatchCollection([],
            color="k", edgecolor="None", alpha=0.5,
            zorder=1, picker=5)
        ax.add_collection(self.array_detectors)

        self.array_signals = PatchCollection([],
            zorder=2, picker=5, alpha=0.8)
        ax.add_collection(self.array_signals)

        # main particle
        self.impact_marker = Line2D([],[],
            marker="o", linestyle="None", color="k",
            markeredgecolor="None",
            markersize=10, zorder=3, alpha=0.8)
        ax.add_line(self.impact_marker)
        self.shower_arrow = FancyArrowPatch((0,0), (0,0),
            arrowstyle="-|>,head_length=10,head_width=5",
            shrinkA=2, shrinkB=2, color="k",
            lw=3, alpha=0.8, zorder=3)
        ax.add_artist(self.shower_arrow)

        # 2nd particle
        self.impact_marker2 = Line2D([],[],
            marker="o", linestyle="None", color="r",
            markeredgecolor="None",
            markersize=10, zorder=3, alpha=0.8)
        ax.add_line(self.impact_marker2)
        self.shower_arrow2 = FancyArrowPatch((0,0), (0,0),
            arrowstyle="-|>,head_length=10,head_width=5",
            shrinkA=2, shrinkB=2, color="r",
            lw=3, alpha=0.8, zorder=3)
        ax.add_artist(self.shower_arrow2)

        # 3rd particle
        self.impact_marker3 = Line2D([],[],
            marker="o", linestyle="None", color="b",
            markeredgecolor="None",
            markersize=10, zorder=3, alpha=0.8)
        ax.add_line(self.impact_marker3)
        self.shower_arrow3 = FancyArrowPatch((0,0), (0,0),
            arrowstyle="-|>,head_length=10,head_width=5",
            shrinkA=2, shrinkB=2, color="b",
            lw=3, alpha=0.8, zorder=3)
        ax.add_artist(self.shower_arrow3)

        self.array_selection_marker = ax.scatter([], [],
            s=200, marker="s", facecolor="None",
            edgecolor="r", linewidths=3, zorder=10)
        ax.add_artist(self.array_selection_marker)

        ax.set_xticks((-600, -400, -200, 0, 200, 400, 600))
        ax.set_yticks(ax.get_xticks())
        ax.set_xlabel("x / m")
        ax.set_ylabel("y / m", labelpad=-10)
        self.array_legend_top_left = ax.text(
            0.02, 0.98, "", ha="left", va="top",
            fontsize="large",
            transform=ax.transAxes)
        self.array_legend_top_right = ax.text(
            0.98, 0.975, "", ha="right", va="top",
            fontsize="large",
            transform=ax.transAxes)
        self.array_legend_bottom_left = ax.text(
            0.02, 0.02, "", ha="left", va="bottom",
            fontsize=None,
            transform=ax.transAxes)
        self.array_legend_bottom_right = ax.text(
            0.98, 0.02, "", ha="right", va="bottom",
            fontsize="large",
            transform=ax.transAxes)

        ### top right: LDF
        ax = self.ax_ldf = fig.add_axes((0.58, 0.55, 0.40, 0.42),
                                        autoscale_on=False)
        ax.axvline(0, ls="--", color="0.5", zorder=0)
        self.ldf_model = []
        self.ldf_model_err = []
        self.r125_marker = []
        for ib in (0, 1):
            l = Line2D([], [], color="k", zorder=1)
            self.ldf_model.append(l)
            ax.add_line(l)
            p = Polygon([[0,0]], closed=True, facecolor="0.8", edgecolor="0.5", zorder=0)
            self.ldf_model_err.append(p)
            ax.add_patch(p)
            m = Line2D([], [], marker="o", ms=10, color="k", zorder=0)
            self.r125_marker.append(m)
            ax.add_line(m)
        self.tank.ldf_points = ax.scatter([],[], zorder=2, picker=1)
        self.tank.ldf_slc_points = ax.scatter([],[], color="None",
                                         edgecolor="k", zorder=1)
        self.scint.ldf_points = ax.scatter([],[], marker="s", zorder=2, picker=1)
        self.scint.ldf_slc_points = ax.scatter([],[], color="None", marker="s",
                                               edgecolor="k", zorder=1)
        self.ldf_nohit_points = ax.scatter([],[], color="None",
                                           marker="v",
                                           edgecolor="0.5", zorder=0)
        self.ldf_selection_marker = ax.scatter([], [],
            s=200, marker="s", facecolor="None",
            edgecolor="r", linewidths=3, zorder=10)
        ax.set_ylim(5e-2, 5e3)
        ax.set_yscale("log")
        msetp(ax.get_xticklabels(), visible=False)
        ax.set_ylabel("S / VEM")
        self.ldf_legend = ax.text(
            0.98, 0.98, "", ha="right", va="top",
            fontsize="large",
            transform=ax.transAxes)

        ### bottom right: Time
        ax = self.ax_time = fig.add_axes((0.58, 0.10, 0.40, 0.42),
                                         sharex=self.ax_ldf,
                                         autoscale_on=False)
        ax.axvline(0, ls="--", color="0.5", zorder=0)
        self.time_model = Line2D([], [], color="k", zorder=1)
        ax.add_line(self.time_model)
        self.time_model_err = Polygon([[0,0]], closed=True, facecolor="0.8", edgecolor="0.5", zorder=0)
        ax.add_patch(self.time_model_err)
        self.tank.time_points = ax.scatter([],[], zorder=2, picker=2)
        self.tank.time_slc_points = ax.scatter([],[], color="None",
                                          edgecolor="k", zorder=2)
        self.scint.time_points = ax.scatter([],[], marker="s", zorder=2, picker=2)
        self.scint.time_slc_points = ax.scatter([],[], color="None", marker="s",
                                                edgecolor="k", zorder=2)
        self.time_selection_marker = ax.scatter([], [],
            s=200, marker="s", facecolor="None",
            edgecolor="r", linewidths=3, zorder=10)
        ax.add_artist(self.time_selection_marker)
        ax.set_xlabel("r / m")
        ax.set_ylabel("(t[front] - t) / us")
        ax.set_xlim(100 if settings["log-radius"] else -600, 600)
        ax.set_ylim(-1.0, 0.0)
        self.side_labels = (
            ax.text(0.49, 0.03, "early side", ha="right",
                    color="0.5", transform=ax.transAxes),
            ax.text(0.51, 0.03, "late side", ha="left",
                    color="0.5", transform=ax.transAxes)
        )

        self.reset_event_data(settings)

        self.mpl_connect("pick_event", self.on_pick)
        self.mpl_connect("scroll_event", self.reset_zoom)

    def reset_event_data(self, settings):
        self.settings = settings
        self.detector_data = []
        self.tank.pulse_index = []
        self.scint.pulse_index = []

        # reset select
        self.array_selection_marker.set_offsets([])
        self.ldf_selection_marker.set_offsets([])
        self.time_selection_marker.set_offsets([])
        self.ldf_nohit_points.set_offsets([])
        self.array_legend_bottom_left.set_text("")
        if self.ax_trace:
            self.ax_trace.cla()

    def handle_title(self, frame, show_time_trace):
        if frame and eventheader_key in frame:
            evth = frame[eventheader_key]
            font = FontProperties(variant="small-caps",
                                  weight="bold")
            tx = "Run {}    Event {}    SubEvent {}"\
                 .format(evth.run_id,
                         evth.event_id,
                         evth.sub_event_id)
            if not show_time_trace: # save space
                tx += "\n" + str(evth.start_time)
            self.ax_array.set_title(tx,
                fontproperties=font, y=1.02)
        else:
            self.ax_array.set_title("")

    def handle_legends(self, rec_data, settings, nhit, have_particle):
        particle = rec_data["particle"]
        p_pos = rec_data["pos"]
        if settings["show legends"] and have_particle:
            zenith = particle.dir.zenith
            azimuth = particle.dir.azimuth
            if particle.type != particle.ParticleType.unknown:
                tx = "Primary: {:s}\n".format(str(particle.type))
                tx += "E$_{{true}}$: {:.1f} PeV\n".format(particle.total_energy/I3Units.PeV)
            else:
                tx = "Fit: " + str(particle.fit_status)
            self.array_legend_top_left.set_text(
                "core = ({:.0f}, {:.0f}) m\n"
                .format(p_pos[0], p_pos[1])
                + tx
            )
            self.array_legend_top_right.set_text(
                "$\\theta$ = {:.1f}$^\\circ$\n"
                "$\\phi$ = {:.1f}$^\\circ$"
                .format(zenith / I3Units.degree,
                        azimuth / I3Units.degree)
            )
            self.array_legend_bottom_right.set_text(
                "{} hits\n"
                "$\chi^2/n_\\mathrm{{dof}}$ = {:.1f}"
                .format(nhit, rec_data["rchi2"])
            )
            self.ldf_legend.set_text(
                "S$_{{125}}$ = {:.1f} VEM\n"
                "$\\beta$ = {:.1f}\n"
                .format(rec_data["s125"][0],
                        rec_data["beta"][0])
            )
        else:
            self.array_legend_top_left.set_text("")            
            self.array_legend_top_right.set_text("")
            self.array_legend_bottom_right.set_text("")
            self.ldf_legend.set_text("")

    def handle_time_trace(self, show_time_trace, detector_data):
        fig = self.figure
        if show_time_trace and self.ax_trace is None:
            self.ax_trace = fig.add_axes((0.1, 0.1, 0.4, 0.4),
                                         autoscale_on=False)
            w_inch, h_inch = fig.get_size_inches()
            self.resize_event(w_inch * fig.dpi, h_inch * fig.dpi)
        elif not show_time_trace and self.ax_trace:
            if self.ax_trace in self.xlim:
                del self.xlim[self.ax_trace]
                del self.ylim[self.ax_trace]
            self.ax_trace.cla()
            fig.delaxes(self.ax_trace)
            self.ax_trace = None
            w_inch, h_inch = fig.get_size_inches()
            self.resize_event(w_inch * fig.dpi, h_inch * fig.dpi)

        if self.ax_trace:
            self.ax_trace.cla()
            # draw a trace overview here, only the best trace per tank
            is_dom_launch = False
            I3Waveform = dataclasses.I3Waveform
            I3DOMLaunch = dataclasses.I3DOMLaunch
            vmax = 0.0
            for tank in detector_data:
                if is_dom_launch: break
                best_traces = []
                for igain, gain in enumerate(("HG", "LG")):
                    for trace_obj in tank.doms[gain]["traces"]:
                        vm = None
                        if isinstance(trace_obj, I3DOMLaunch):
                            is_dom_launch = True
                            break
                        elif isinstance(trace_obj, I3Waveform):
                            if trace_obj.source in (I3Waveform.ATWD, I3Waveform.SLC):
                                v = np.array(trace_obj.waveform) / I3Units.mV
                                vm = np.max(v)
                                t = trace_obj.time + trace_obj.bin_width * np.arange(len(v) + 1)
                        if vm is not None: # found a candidate
                            # adjust output of low gain DOM to high gain DOM, factor
                            # is gain difference according to IceTop detector paper
                            if gain == "LG":
                                v *= 50
                                vm *= 50

                            # compare traces with the same start time and select
                            # trace based on the maximum value, which is higher
                            # for traces less affected by saturation
                            for ibest, best in enumerate(best_traces):
                                if abs(best[0][0] - t[0]) < 100.0:
                                    if best[2] < vm:
                                        best_traces[ibest] = (t, v, vm, gain)
                                    break
                            else:
                                best_traces.append((t, v, vm, gain))

                for t, v, vm, gain in best_traces:
                    is_slc = len(v) == 1
                    color = "k"
                    if gain == "LG":
                        color = "r"
                    if is_slc:
                        color = "b"
                    tr = mpl_hist(t, v,
                                  facecolor=color,
                                  edgecolor="None",
                                  alpha=0.1 if is_slc else 0.3,
                                  zorder=1 if is_slc else 2)
                    self.ax_trace.add_artist(tr)

            ax = self.ax_trace
            if is_dom_launch:
                ax.xaxis.set_ticks([])
                ax.yaxis.set_ticks([])
                ax.text(0.5, 0.5,
                        "Click on individual tanks to see raw traces.",
                        ha="center", va="center", color="k",
                        fontsize="large",
                        transform=ax.transAxes)
            else:
                for i, item in enumerate((("HG", "k"),
                                          ("LG", "r"),
                                          ("SLC", "b"))):
                    label, color = item
                    ax.text(0.81 + 0.06 * i, 0.98, 
                            label, color=color,
                            va="top", transform=ax.transAxes)
                ax.set_yscale("symlog", linthresy=1.0)
                ax.set_ylim(0.0, 1e5)
                ax.set_xlim(self.tmin, self.tmax)
                ax.set_xlabel("t / ns")
                ax.set_ylabel("signal / mV")


    def update_frame(self, frame, keys, settings):
        self.reset_event_data(settings)
        show_legends = settings["show legends"]
        show_nohits = settings["show no-hits"]
        use_log_radius = settings["log-radius"]
        time_trace_key = settings["time trace"]
        show_time_trace = bool(time_trace_key)
        self.handle_title(frame, show_time_trace)

        part_key = settings["particle"]
        slc_key = settings["SLCs"]
        scint_hlc_key = settings["scint HLCs"]
        scint_slc_key = settings["scint SLCs"]        
        keys += [part_key, time_trace_key, slc_key,
                 scint_hlc_key, scint_slc_key]
        # keep detector_data around, used by on_pick
        rec_data, self.detector_data = get_data(frame, keys, use_log_radius)

        self.handle_time_trace(show_time_trace,
                               self.detector_data)

        have_particle = (rec_data["particle"].fit_status != 
                         dataclasses.I3Particle.FitStatus.NotSet or
                         rec_data["particle"].type !=
                         dataclasses.I3Particle.unknown)

        ## array plot
        signal_patches = []
        det_patches = []
        r_hit_tank = []
        s_hit_tank = []
        t_plane_tank = []
        r_slc_tank = []
        s_slc_tank = []
        t_slc_tank = []
        r_nohit = []
        r_hit_scint = []
        s_hit_scint = []
        t_plane_scint = []
        r_slc_scint = []
        s_slc_scint = []
        t_slc_scint = []
        scale = settings["scale"]
        power = settings["power"]
        def pulse_cmp(a, b):
            "Put HLC pulses in front of SLC pulses, sort both in time."
            v = int(b.hlc) - int(a.hlc)
            if v == 0:
                return int(a.t - b.t)
            return v
        for d in self.detector_data:
            is_scint = isinstance(d, ScintData)
            pos = d.pos
            ch_tank = 0.0
            # use charge and time of first pulse
            pulses = d.pulses
            pulse_ind_scint = []
            pulse_ind_tank = []
            if pulses:
                # for the tank display, use the first HLC hit,
                # if there are no HLC hits, use the first SLC hit
                pulses.sort(pulse_cmp)
                ch_tank = pulses[0].charge
                if is_scint:
                    r_hit = r_hit_scint
                    s_hit = s_hit_scint
                    t_plane = t_plane_scint
                    r_slc = r_slc_scint
                    s_slc = s_slc_scint
                    t_slc = t_slc_scint
                    pulse_ind = pulse_ind_scint
                else:
                    r_hit = r_hit_tank
                    s_hit = s_hit_tank
                    t_plane = t_plane_tank
                    r_slc = r_slc_tank
                    s_slc = s_slc_tank
                    t_slc = t_slc_tank
                    pulse_ind = pulse_ind_tank
                for pulse in pulses:
                    pulse_ind.append(len(t_plane))
                    ch = pulse.charge
                    r = d.r
                    r_hit.append(abs(r) if use_log_radius else r)
                    s_hit.append(ch)
                    t = pulse.t_plane
                    t_plane.append(t / I3Units.microsecond)
                    if not pulse.hlc:
                        r_slc.append(r)
                        s_slc.append(ch)
                        t_slc.append(t / I3Units.microsecond)
            else:
                if show_nohits:
                    r_nohit.append(d.r)
            self.tank.pulse_index.append(pulse_ind_tank)
            self.scint.pulse_index.append(pulse_ind_scint)
            size = scale * ch_tank ** power
            if is_scint: # scintillator
                spatch = Rectangle(pos - 0.5 * size, size, size)
                dpatch = Rectangle(pos - 5, 10, 10)
            else:
                ctr = d.ctr
                phi = d.phi
                cp = np.cos(np.radians(phi + 90))
                sp = np.sin(np.radians(phi + 90))
                # workaround for weird matplotlib-1.3 bug in drawing signal_patches
                if phi > 0.0:
                    phi -= 360.0
                spatch = Wedge(ctr, size, phi, phi + 180.0)
                dpatch = Circle(pos, 5)
            signal_patches.append(spatch)
            det_patches.append(dpatch)
        self.array_detectors.set_paths(det_patches)
        self.array_signals.set_paths(signal_patches)

        p_pos = rec_data["pos"][:2]
        p_dir = rec_data["dir"][:2]
        self.impact_marker.set_data(*p_pos)
        self.shower_arrow.set_positions(p_pos - 600 * p_dir, p_pos)
        if p_dir[0] == 0.0 and p_dir[1] == 0.0:
            perp = np.array((0.0, 1.0))
        elif abs(p_dir[0]) > abs(p_dir[1]):
            perp = np.array((1.0, -p_dir[0] / p_dir[1]  ))
        else:
            perp = np.array((-p_dir[1] / p_dir[0], 1.0))
        perp /= np.sum(perp ** 2)
        p0 = p_pos - 1e6 * perp
        p1 = p_pos + 1e6 * perp
        self.shower_plane_intersection.set_data((p0[0], p1[0]), (p0[1], p1[1]))

        self.handle_legends(rec_data, settings, len(t_plane_tank), have_particle)

        ## LDF & time plots
        if use_log_radius and r_hit:
            self.ax_ldf.set_xscale("log")
            self.ax_time.set_xscale("log")
            for l in self.side_labels:
                l.set_visible(False)
        else:
            self.ax_ldf.set_xscale("linear")
            self.ax_time.set_xscale("linear")
            for l in self.side_labels:
                l.set_visible(True)

        for det in (self.tank, self.scint):
            msize = [settings["marker-size"] ** 2]
            set_sizes(det.ldf_points, msize)
            set_sizes(det.time_points, msize)
            msize = [(settings["marker-size"] * 1.6) ** 2]
            set_sizes(det.ldf_slc_points, msize)
            set_sizes(det.time_slc_points, msize)
        msize = [(settings["marker-size"] * 1.6) ** 2]
        set_sizes(self.ldf_nohit_points, msize)

        self.tank.ldf_points.set_offsets(np.dstack((r_hit_tank, s_hit_tank)))
        self.tank.time_points.set_offsets(np.dstack((r_hit_tank, t_plane_tank)))
        self.tank.ldf_slc_points.set_offsets(np.dstack((r_slc_tank, s_slc_tank)))
        self.tank.time_slc_points.set_offsets(np.dstack((r_slc_tank, t_slc_tank)))
        self.scint.ldf_points.set_offsets(np.dstack((r_hit_scint, s_hit_scint)))
        self.scint.time_points.set_offsets(np.dstack((r_hit_scint, t_plane_scint)))
        self.scint.ldf_slc_points.set_offsets(np.dstack((r_slc_scint, s_slc_scint)))
        self.scint.time_slc_points.set_offsets(np.dstack((r_slc_scint, t_slc_scint)))

        for ib in (0, 1):
            self.ldf_model[ib].set_data([], [])
            self.ldf_model_err[ib].set_xy([[0,0]])
            self.r125_marker[ib].set_data([], [])
        self.time_model.set_data([], [])
        self.time_model_err.set_xy([[0,0]])

        # if we have a reconstruction, draw related items
        if have_particle:
            r_ldf = rec_data["rspace"]
            v_ldf = rec_data["ldf"]
            e_ldf = rec_data["ldf_err"]
            s125 = rec_data["s125"]
            rmin = r_ldf[0]
            rmax = r_ldf[-1]
            self.ax_ldf.set_xlim(rmin if use_log_radius else -rmax, rmax)

            for ib, branch in enumerate((-1, 1)):
                if use_log_radius and branch < 0:
                    continue
                self.ldf_model[ib].set_data(branch * r_ldf, v_ldf)
                self.ldf_model_err[ib].set_xy(band_xy(branch * r_ldf, v_ldf, e_ldf))
                self.r125_marker[ib].set_data([branch * 125], s125[0])

            r_time = r_ldf
            v_time = rec_data["curv"] / I3Units.microsecond
            e_time = rec_data["curv_err"] / I3Units.microsecond
            if not use_log_radius:
                r2 = -r_ldf[::-1]
                v2 = v_time[::-1]
                e2 = e_time[::-1]
                r_time = np.append(r2, r_ldf)
                v_time = np.append(v2, v_time)
                e_time = np.append(e2, e_time)
            self.time_model.set_data(r_time, v_time)
            self.time_model_err.set_xy(band_xy(r_time, v_time, e_time))

            if pulses and len(t_plane) >= 2:
                t_min = min(-0.2, np.min(t_plane))
                t_max = max(0.0, np.max(t_plane))
                t_min -= 0.1 * (t_max - t_min)
                t_max += 0.2 * (t_max - t_min)
                self.ax_time.set_ylim(t_min, t_max)

            s_nohit = []
            for i,r in enumerate(r_nohit):
                s = 1.5 / abs(rec_data["dir"][2])
                s_nohit.append(s)
            self.ldf_nohit_points.set_offsets(np.dstack((r_nohit, s_nohit)))

        self.update_colors(self.tmin, self.tmax)

        self.update_secondary_particles(frame, settings)

    def update_colors(self, tmin, tmax):
        if tmin == tmax: return
        self.tmin = tmin
        self.tmax = tmax
        if self.ax_trace:
            self.ax_trace.set_xlim(tmin, tmax)
        cmap = self.settings["colormap"]
        scint_cmap = self.settings["scintillator colormap"]
        facecolors = []
        edgecolors = []
        for i, d in enumerate(self.detector_data):
            if d.pulses:
                t = d.pulses[0].t
                z = (t - tmin) / (tmax - tmin)
                if z > 1.0: z = 1.0
                elif z < 0.0: z = 0.0
                if isinstance(d, ScintData): facecolors.append(scint_cmap.value(z).rgbF())
                else: facecolors.append(cmap.value(z).rgbF())
                edgecolors.append((0.0, 0.0, 0.0))
            else:
                facecolors.append((1.0, 1.0, 1.0))
                edgecolors.append((0.7, 0.7, 0.7))
        self.array_signals.set_facecolors(facecolors)
        self.array_signals.set_edgecolors(edgecolors)

        for det in (self.tank, self.scint):
            fcs = []
            for i, pulse_ind in enumerate(det.pulse_index):
                for j in pulse_ind:
                    fcs.append(facecolors[i])
            det.ldf_points.set_facecolors(fcs)
            det.time_points.set_facecolors(fcs)

    def update_secondary_particles(self, frame, settings):
        key2 = settings["2nd particle"]
        key3 = settings["3rd particle"]
        for ikey, key in enumerate((key2, key3)):
            p_pos = np.array((1e100, 1e100))
            p_dir = np.array((0.0, 0.0))
            if key and frame and key in frame:
                part = frame[key]
                p_pos2 = np.array((part.pos.x, part.pos.y))
                p_dir2 = np.array((part.dir.x, part.dir.y))
                if np.all(~np.isnan(p_pos2 * p_dir2)):
                    p_pos = p_pos2
                    p_dir = p_dir2
            impact_marker = getattr(self, "impact_marker{0}".format(ikey + 2))
            shower_arrow = getattr(self, "shower_arrow{0}".format(ikey + 2))
            impact_marker.set_data(*p_pos)
            shower_arrow.set_positions(p_pos - 600 * p_dir,
                                       p_pos)

    def resize_event(self, w, h):
        if w == 0 or h == 0: return
        fig = self.figure
        w_inch = float(w) / fig.dpi
        h_inch = float(h) / fig.dpi
        fig.set_size_inches(w_inch, h_inch)

        # adjust all axes positions
        left1 = 55.0 / w
        left2 = 75.0 / w
        hspace = 15.0 / h
        right = 1.0 - 15.0 / w
        top1 = 1.0 - 50.0 / h
        top2 = 1.0 - 10.0 / h
        bottom = 50.0 / h
        if self.ax_trace is None:
            self.ax_array.set_position((
                left1, bottom, 0.5 - left1, top1 - bottom
            ))
        else:
            top1 = 1.0 - 25.0 / h
            hax = 0.5 * (top1 - 2 * bottom - hspace)
            self.ax_array.set_position((
                left1, 2 * bottom + hspace + hax,
                0.5 - left1, hax
            ))
            self.ax_trace.set_position((
                left1, bottom, 0.5 - left1, hax
            ))            
        wax = right - 0.5 - left2
        hax = 0.5 * (top2 - bottom - hspace)
        self.ax_ldf.set_position((
            0.5 + left2, bottom + hax + hspace,
            wax, hax
        ))
        self.ax_time.set_position((
            0.5 + left2, bottom,
            wax, hax
        ))

        # adjust aspect of array plot
        ax = self.ax_array
        bbox = ax.get_position()
        wax = bbox.width * w
        hax = bbox.height * h
        r = wax / hax
        if 1.0 < r:
            ax.set_xlim(-650 * r, 650 * r)
            ax.set_ylim(-650, 650)
        else:
            ax.set_xlim(-650, 650)
            ax.set_ylim(-650 / r, 650 / r)

    def zoom_event(self, x, y, w, h):
        zoom_box = Bbox.from_bounds(x, y, w, h)

        # find intersecting bbox
        match = None
        for ax in self.figure.get_children():
            if not isinstance(ax, Axes):
                continue

            axes_box = ax.get_window_extent()
            box = Bbox.intersection(zoom_box, axes_box)
            if box is None:
                continue

            if match is not None:
                # more than one match, don't do anything
                return

            match = ax, box
        if match is None:
            return

        ax, box = match
        xlim = ax.get_xlim()
        ylim = ax.get_ylim()

        if ax is self.ax_ldf or ax is self.ax_time:
            # ax_ldf and ax_time share the x-axis
            if ax not in self.xlim:
                self.xlim[self.ax_ldf] = xlim
                self.xlim[self.ax_time] = xlim
            if ax not in self.ylim:
                self.ylim[ax] = ylim
        else:
            if ax not in self.xlim:
                self.xlim[ax] = xlim
                self.ylim[ax] = ylim

        trans = ax.transData.inverted()
        box = trans.transform(box)
        (x0, y0), (x1, y1) = box
        if ax is self.ax_array: # preserve aspect ratio
            original_xlim = self.xlim[ax]
            original_ylim = self.ylim[ax]
            r0 = ((original_xlim[1] - original_xlim[0]) /
                  (original_ylim[1] - original_ylim[0]))
            r1 = (x1 - x0) / (y1 - y0)
            if r1 > r0:
                dy = 0.5 / r0 * (x1 - x0)
                y = 0.5 * (y0 + y1)
                ax.set_xlim(x0, x1)
                ax.set_ylim(y - dy, y + dy)
            else:
                dx = 0.5 * r0 * (y1 - y0)
                x = 0.5 * (x0 + x1)
                ax.set_xlim(x - dx, x + dx)
                ax.set_ylim(y0, y1)
        else:
            ax.set_xlim(x0, x1)
            ax.set_ylim(y0, y1)

    def reset_zoom(self, mouseevent):
        def get(t, ax, tup):
            a, b = tup
            scale = getattr(ax, "get_{0}scale".format(t))()
            if scale == "log":
                return np.log(a), np.log(b)
            else:
                return a, b

        def set(t, ax, a, b):
            scale = getattr(ax, "get_{0}scale".format(t))()
            if scale == "log":
                a = np.exp(a)
                b = np.exp(b)
            getattr(ax, "set_{0}lim".format(t))(a, b)

        for ax in self.figure.get_children():
            if not isinstance(ax, Axes):
                continue
            if not ax.in_axes(mouseevent):
                continue

            if mouseevent.step < 0 and ax in self.xlim and ax in self.ylim:
                factor = 1.0 - 0.5 * mouseevent.step
                x0, x1 = get("x", ax, ax.get_xlim())
                y0, y1 = get("y", ax, ax.get_ylim())
                xm = 0.5 * (x0 + x1)
                ym = 0.5 * (y0 + y1)
                w = (x1 - x0) * factor
                h = (y1 - y0) * factor
                x0 = xm - 0.5 * w
                x1 = xm + 0.5 * w
                y0 = ym - 0.5 * h
                y1 = ym + 0.5 * h

                ox0, ox1 = get("x", ax, self.xlim[ax])
                oy0, oy1 = get("y", ax, self.ylim[ax])
                ow = ox1 - ox0
                oh = oy1 - oy0
                if ax is self.ax_array:
                    if w > ow or h > oh:
                        x0 = ox0
                        x1 = ox1
                        y0 = oy0
                        y1 = oy1
                else:
                    if w > ow:
                        x0 = ox0
                        x1 = ox1
                    if h > oh:
                        y0 = oy0
                        y1 = oy1
                set("x", ax, x0, x1)
                set("y", ax, y0, y1)
                return

    def on_pick(self, pickevent):
        if pickevent.mouseevent.button != 1: return

        # update selection markers
        self.ldf_selection_marker.set_offsets([])
        self.time_selection_marker.set_offsets([])
        picked_detector = None
        picked_pulse = None
        if pickevent.artist in (self.array_signals, self.array_detectors):
            ti = pickevent.ind[0]
            picked_detector = self.detector_data[ti]
            xy = picked_detector.pos
            self.array_selection_marker.set_offsets([xy])

            for det in (self.tank, self.scint):
                pi_list = det.pulse_index[ti]
                if pi_list:
                    self.ldf_selection_marker.set_offsets(
                        [det.ldf_points.get_offsets()[pi] for pi in pi_list]
                    )
                    self.time_selection_marker.set_offsets(
                        [det.time_points.get_offsets()[pi] for pi in pi_list]
                    )
                    break
        else:
            for det in (self.tank, self.scint):
                if pickevent.artist in (det.ldf_points, det.time_points):
                    pi = pickevent.ind[0]
                    xy = det.ldf_points.get_offsets()[pi]
                    self.ldf_selection_marker.set_offsets([xy])
                    xy = det.time_points.get_offsets()[pi]
                    self.time_selection_marker.set_offsets([xy])
                    t_point = xy[1]
                    for ti, pis in enumerate(det.pulse_index):
                        if pi in pis:
                            picked_detector = self.detector_data[ti]
                            xy = picked_detector.pos
                            self.array_selection_marker.set_offsets([xy])
                            for p in picked_detector.pulses:
                                t_plane = p.t_plane / I3Units.microsecond
                                if abs(t_plane - t_point) < 0.1:
                                    picked_pulse = p.t
                                    break
                            break

        # update detector info
        if isinstance(picked_detector, ScintData):
            self.array_legend_bottom_left.set_text(
                "Scint {0}".format(picked_detector.label)
            )
        else:
            self.array_legend_bottom_left.set_text(
                ("Tank {0}\n"
                 "{1} liner\n"
                 "h[snow] = {2:.1f} m")
                .format(picked_detector.label,
                        str(picked_detector.type).strip("_Lined"),
                        picked_detector.h_snow)
            )

        # update traces
        self.on_pick_update_traces(picked_detector, picked_pulse)

    def on_pick_update_traces(self, detector, pulse_time):
        ax = self.ax_trace
        if not ax: return
        ax.cla()
        ax.xaxis.set_ticks_position("bottom")
        tmin = np.inf
        tmax = -np.inf

        # draw time of pulses for comparison
        cmap = self.settings["colormap"]
        for p in detector.pulses:
            t = p.t
            z = (t - self.tmin) / (self.tmax - self.tmin)
            if z > 1.0: z = 1.0
            elif z < 0.0: z = 0.0
            ax.scatter([t], [0], s=80,
                       marker="o",
                       facecolor=cmap.value(z).rgbF(),
                       edgecolor="k",
                       zorder=20)
            tmin = min(tmin, t - 10)
            tmax = max(tmax, t + 10)

        show_fadc = self.settings["show FADC"]
        show_legends = self.settings["show legends"]
        I3Waveform = dataclasses.I3Waveform
        I3DOMLaunch = dataclasses.I3DOMLaunch
        is_dom_launch = False
        chip_texts = []
        for gain in detector.doms:
            gsign = -1 if gain == "LG" else 1
            dom = detector.doms[gain]
            if "traces" not in dom:
                continue

            # if we have waveforms, show only the best per DOM
            best_waveforms = []
            for trace_obj in dom["traces"]:

                # match traces against selected pulse time
                if (pulse_time is not None and
                    abs(trace_obj.time - pulse_time) > 500):
                    continue

                if isinstance(trace_obj, I3DOMLaunch):
                    is_dom_launch = True
                    this_tmin = np.inf
                    this_tmax = -np.inf

                    atwd_channels = trace_obj.raw_atwd
                    # draw ATWD channels
                    for i in range(len(atwd_channels)):
                        channel = atwd_channels[i]
                        if len(channel) == 0:
                            break
                        v = gsign * np.array(channel)
                        t = trace_obj.time + atwd_tbin * np.arange(len(v) + 1)
                        this_tmin = min(this_tmin, t[0])
                        this_tmax = max(this_tmax, t[-1])
                        ax.add_artist(mpl_hist(t, v,
                                               facecolor="krbg"[i],
                                               edgecolor="None",
                                               alpha=0.6,
                                               zorder=2 + i))
                        ax.add_artist(mpl_hist(t, v,
                                               facecolor="None",
                                               edgecolor="krbg"[i],
                                               zorder=9 - i))

                    # if no channels, try to draw the charge stamp (SLCs)
                    if this_tmin > this_tmax:
                        charge = trace_obj.raw_atwd_charge_stamp
                        # ATWD has 128 bins
                        v = gsign * np.array([charge / 128])
                        t = trace_obj.time + atwd_tbin * 129 * np.arange(2)
                        this_tmin = min(this_tmin, t[0])
                        this_tmax = max(this_tmax, t[-1])
                        i = trace_obj.which_atwd_charge_stamp
                        ax.add_artist(mpl_hist(t, v,
                                               facecolor="None",
                                               edgecolor="krbg"[i],
                                               hatch="//",
                                               linewidth=2,
                                               alpha=0.8,
                                               zorder=2))

                    # which chip, lc_bit
                    if this_tmin < this_tmax and show_legends:
                        tx = ax.text(trace_obj.time,
                                     1100 if gsign > 0 else -1250,
                                     (" {0}, LC " if trace_obj.lc_bit else " {0} ")
                                     .format(trace_obj.which_atwd))
                        chip_texts.append(tx)
                        ax.plot((trace_obj.time, trace_obj.time), 
                                (0, gsign * 1300), ":k")

                    # draw FADC
                    if show_fadc:
                        v = gsign * np.array(trace_obj.raw_fadc)
                        t = trace_obj.time + fadc_tbin * np.arange(len(v) + 1)
                        this_tmin = min(this_tmax, t[0])
                        this_tmax = max(this_tmax, t[-1])
                        ax.add_artist(mpl_hist(t, v,
                                               facecolor="c",
                                               edgecolor="None",
                                               zorder=1))
                        ax.add_artist(mpl_hist(t, v,
                                               facecolor="None",
                                               edgecolor="c",
                                               zorder=10))

                    tmin = min(this_tmin, tmin)
                    tmax = max(this_tmax, tmax)

                elif isinstance(trace_obj, I3Waveform):
                    if trace_obj.source == I3Waveform.FADC:
                        continue
                    v = np.array(trace_obj.waveform) / I3Units.mV
                    vm = v.max()
                    v *= gsign
                    t = trace_obj.time + trace_obj.bin_width * np.arange(len(v) + 1)

                    # only keep best waveform of all ATWD channels
                    for i, b in enumerate(best_waveforms):
                        if abs(b[0][0] - t[0]) < 100:
                            # found match
                            if b[2] < vm:
                                best_waveforms[i] = (t, v, vm)
                            break
                    else:
                        best_waveforms.append((t, v, vm))

            # draw best waveform per DOM
            for b in best_waveforms:
                t, v, vm = b
                tmin = min(tmin, t[0])
                tmax = max(tmax, t[-1])
                ax.add_artist(mpl_hist(t, v,
                                       facecolor="k",
                                       edgecolor="None",
                                       alpha=0.5))

        if is_dom_launch:
            ax.set_yscale("linear")
            ax.set_ylim(-1300, 1300)
            ax.set_yticks((-1024, -768, -512, -256, 0, 256, 512, 768, 1024))

            if show_legends:
                strings = ["ATWD channels: ", "0", "1", "2", "3"]
                colors = "kkrbg"
                fontweights = ["normal"] + 4*["bold"]
                if show_fadc:
                    strings += ["  FADC"]
                    colors += "c"
                    fontweights += ["bold"]
                rainbow_text(ax, 0.0, 1.02,
                             strings, colors,
                             fontweights=fontweights,
                             transform=ax.transAxes)
        else:
            ax.set_yscale("symlog", linthresy=1.0)
            ax.set_ylim(-1e5, 1e5)
            ax.set_ylabel("signal / mV")

        ax.text(-0.10, -0.02, "LG", ha="right", va="bottom",
                transform=ax.transAxes)
        ax.text(-0.10, 1.02, "HG", ha="right", va="top",
                transform=ax.transAxes)

        if tmin < tmax:
            ax.set_xlim(tmin, tmax)
            ax.set_xlabel("t / ns")

            for tx in chip_texts:
                x = tx.get_position()[0]
                if abs(x - tmax) < abs(x - tmin):
                    tx.set_horizontalalignment("right")

            ax.axhline(0, color="k", zorder=15)
        else:
            ax.set_xticks([])


def get_data(frame, keys, use_log_radius):
    (geo_key, recopulse_key, part_key, trace_key, slc_key,
     scint_hlc_key, scint_slc_key) = keys
    npa = np.array

    ## fill rec_data
    rec_data = {}
    part = None
    params = None
    if frame and part_key in frame:
        part = frame[part_key]
        if (np.isnan(part.pos.mag2) or 
            np.isnan(part.dir.theta) or 
            np.isnan(part.dir.phi)):
            part = None

        part_key += "Params"
        if part_key in frame:
            params = I3LaputopParams.from_frame(frame, part_key)
            Par = LaputopParameter
            lg_s125 = params.value(Par.Log10_S125)
            lg_s125_err = params.error(Par.Log10_S125)
            s125 = 10 ** lg_s125
            beta = params.value(Par.Beta)
            beta_err = params.error(Par.Beta)
            rec_data["s125"] = (s125,) + sigma_charge_conv(lg_s125, lg_s125_err)
            rec_data["beta"] = (beta, beta_err)
            rec_data["ndof"] = params.ndf
            rec_data["rchi2"] = params.chi2_ldf 
        else:
            rec_data["s125"] = (np.nan, 0.0, 0.0)
            rec_data["beta"] = (np.nan, 0.0)
            rec_data["ndof"] = np.nan
            rec_data["rchi2"] = np.nan

    if part is None:
        part = dataclasses.I3Particle() # dummy
        part.pos = dataclasses.I3Position(1e100,
                                          1e100,
                                          I3Constants.zIceTop)
        part.dir = dataclasses.I3Direction(0, 0, 1)

    p_pos = npa((part.pos.x, part.pos.y, part.pos.z))
    p_time = float(part.time)
    p_dir = npa((part.dir.x, part.dir.y, part.dir.z))

    # move space-time point of core to IceTop altitude
    if p_pos[2] != I3Constants.zIceTop:
        s = (I3Constants.zIceTop - p_pos[2]) / p_dir[2]
        p_pos += p_dir * s
        p_time += s / I3Constants.c

    rec_data["particle"] = part
    rec_data["pos"] = p_pos
    rec_data["dir"] = p_dir

    ## fill detector_data
    detector_data = []
    pos_list = []
    if frame and geo_key in frame:
        geo = frame[geo_key]

        # discover and cache scintillator doms
        if cache.geometry_time_range != (geo.start_time, geo.end_time):
            cache.geometry_time_range = (geo.start_time, geo.end_time)
            cache.scintillator_omkeys = []
            for omkey,omgeo in geo.omgeo.items():
                if omgeo.omtype == dataclasses.I3OMGeo.Scintillator:
                    cache.scintillator_omkeys.append(omkey)

        recopulse_map = {}
        if recopulse_key in frame:
            recopulse_map = frame[recopulse_key]
            if not hasattr(recopulse_map, "keys"): # is a mask
                recopulse_map = recopulse_map.apply(frame)

        slc_map = {}
        if slc_key in frame:
            slc_map = frame[slc_key]
            if not hasattr(slc_map, "keys"): # is a mask
                slc_map = slc_map.apply(frame)

        trace_map = {}
        cal = {}
        detstat = {}
        if trace_key and trace_key in frame:
            trace_map = frame[trace_key]
            # load only if traces are displayed, it's slow
            if calibration_key in frame:
                cal = frame[calibration_key]
            if detectorstatus_key in frame:
                detstat = frame[detectorstatus_key].dom_status

        for station in geo.stationgeo.values():
            pos_sta = np.zeros(3)
            for tank in station:
                pos_sta[0] += tank.position.x
                pos_sta[1] += tank.position.y
                pos_sta[2] += tank.position.z
            pos_sta /= len(station)
            for itank, tank in enumerate(station):
                pos = tank.position
                pos = npa((pos.x, pos.y, pos.z))
                v = pos - pos_sta
                phi = np.arctan2(v[1], v[0]) - 0.5 * np.pi
                label = str(tank.omkey_list[0].string) + "AB"[itank]
                doms = {}
                pulses = []
                for gain in ("LG", "HG"):
                    doms[gain] = {"traces": []}
                for omkey in tank.omkey_list:
                    if omkey in recopulse_map:
                        for pulse in recopulse_map[omkey]:
                            pulses.append(PulseData(pulse.time,
                                                    pulse.charge,
                                                    True))

                    if omkey in slc_map:
                        for pulse in slc_map[omkey]:
                            pulses.append(PulseData(pulse.time,
                                                    pulse.charge,
                                                    False))

                    # TODO: fill stuff from vemCal here as necessary
                    if omkey in trace_map:
                        gain = None
                        if omkey in detstat:
                            if detstat[omkey].dom_gain_type == dataclasses.I3DOMStatus.High:
                                gain = "HG"
                            elif detstat[omkey].dom_gain_type == dataclasses.I3DOMStatus.Low:
                                gain = "LG"
                        if gain is None:
                            # assuming standard gain assignment: HG 61,63, LG 62,64
                            gain = "HG" if omkey.om % 2 else "LG"
                        doms[gain]["traces"] = trace_map[omkey]

                tdata = TankData()
                tdata.type = tank.tanktype
                tdata.ctr = pos_sta[:2]
                tdata.phi = np.degrees(phi)
                tdata.pos = pos[:2]
                tdata.label = label
                tdata.h_snow = tank.snowheight
                tdata.h_ice = tank.fillheight
                tdata.pulses = pulses
                tdata.doms = doms
                detector_data.append(tdata)
                pos_list.append(pos)

        # handle scintillators
        scint_map = {}
        for key in (scint_hlc_key, scint_slc_key):
            if key in frame:
                scmap = frame[key]
                if not hasattr(scmap, "keys"): # is a mask
                    scmap = scmap.apply(frame)
                scint_map.update(scmap)

        for omkey in cache.scintillator_omkeys:
            scint_geo = geo.omgeo[omkey]
            label = "{0}-{1}".format(omkey.string, omkey.om)
            doms = {"HG": {"traces": []}, "LG": {"traces": []}}
            pulses = []
            if omkey in scint_map:
                for pulse in scint_map[omkey]:
                    pulses.append(PulseData(pulse.time,
                                            pulse.charge,
                                            pulse.flags & pulse.PulseFlags.LC))
            sdata = ScintData()
            pos = npa((scint_geo.position.x, scint_geo.position.y, scint_geo.position.z))
            sdata.pos = pos[:2]
            sdata.label = label
            sdata.pulses = pulses
            sdata.doms = doms
            detector_data.append(sdata)
            pos_list.append(pos)

    rot_sh = to_shower_cs(part.dir)

    if detector_data:
        vr, vz = signed_r_z(rot_sh, pos_list, p_pos)

        # get points for ldf and curvature model
        # r-range: cover at least the range around 125 m
        rmin = np.inf
        rmax = -np.inf
        for i, d in enumerate(detector_data):
            if d.pulses:
                r = abs(vr[i])
                rmin = min(rmin, r)
                rmax = max(rmax, r)

        if rmin < rmax:
            rmin = min(rmin, 100.0) * 0.9
            if not use_log_radius and rmin > 10.0:
                rmin = 10
            rmax = max(rmax, 150.0) * 1.1
        else:
            rmin = 10.0
            rmax = 600.0

        rspace = 10 ** np.linspace(np.log10(rmin),
                                   np.log10(rmax), 50)
        rec_data["rspace"] = rspace
        if params is None:
            rec_data["ldf"] = np.zeros_like(rspace)
            rec_data["ldf_err"] = np.zeros_like(rspace)
            rec_data["curv"] = np.zeros_like(rspace)
            rec_data["curv_err"] = np.zeros_like(rspace)
        else:
            rec_data["ldf"] = np.vectorize(params.expected_signal)(rspace)
            rec_data["ldf_err"] = np.vectorize(params.expected_signal_error)(rspace)
            rec_data["curv"] = np.vectorize(params.expected_shower_front_delay)(rspace)
            rec_data["curv_err"] = np.vectorize(params.expected_shower_front_delay_error)(rspace)
    else:
        vr = vz = []

    # add time offset to plane shower front
    for d, r, z in zip(detector_data, vr, vz):
        d.r = r
        d.z = z
        for pulse in d.pulses:
            pulse.t_plane = p_time - z / I3Constants.c - pulse.t

    return (rec_data, detector_data)

def sigma_charge_conv(lgs, lgs_err):
    return 10 ** (lgs - lgs_err), 10 ** (lgs + lgs_err)

def band_xy(x, y, yerr):
    return np.append(np.column_stack((x, y - yerr)),
                     np.column_stack((x, y + yerr))[::-1], axis=0)
