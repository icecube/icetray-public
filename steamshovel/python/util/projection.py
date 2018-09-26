import os
import sys
import math
import tempfile
import shutil
from functools import partial

# try importing Pillow before PIL
try:
    import PIL.Image as Image
except ImportError:
    import Image

from icecube import dataclasses
from icecube.shovelart import vec3d
from icecube.icetray import logging

def screenshot(filename):
    from __main__ import window
    window.captureActiveView(filename)

def cross(v1, v2):
    x = v1.y*v2.z - v1.z*v2.y
    y = v1.z*v2.x - v1.x*v2.z
    z = v1.x*v2.y - v1.y*v2.x
    return vec3d(x, y, z)

class ProjectionRenderer:
    def __init__(self, gl, dpi, scale, gamma):
        self.gl = gl
        self.dpi = dpi
        self.scale = scale
        self.gamma = gamma

    def __enter__(self):
        # store current gl settings
        self.ice = None
        for a in self.gl.scenario.getArtists():
            if a.description == 'Ice':
                self.ice = a
        self.base_pos = self.gl.cameraLoc
        self.base_orientation = self.gl.cameraOrientation
        self.base_pivot = self.gl.cameraPivot
        self.base_perspective = self.gl.perspectiveView
        return self

    def __exit__(self, type, value, traceback):
        self.gl.perspectiveView = self.base_perspective
        if self.ice is not None:
            self.gl.scenario.setIsActive(self.ice,True)
        self.gl.setCameraPivot(self.base_pivot)
        self.gl.setCameraLoc(self.base_pos)
        self.gl.setCameraOrientation(self.base_orientation)

    def render(self, perspective, pivot, loc, up, res, filename):
        #setting the pivot modifies the camera location, so it must be done first
        pivot = vec3d(*pivot)
        loc = vec3d(*loc)
        up = vec3d(*up)
        self.gl.perspectiveView = perspective
        self.gl.setCameraPivot(pivot)
        self.gl.setCameraLoc(loc)
        self.gl.setCameraOrientation(cross(pivot-loc, up))
        self.gl.screenshot_engine(res, res, filename, self.dpi, self.scale, self.gamma)


def get_view(gl, frame):
    # get track key
    line = None
    for a in gl.scenario.getArtists():
        if not a.isActive:
            logging.log_warn("Ignoring inactive Artist {} with keys {}".format(a.description,a.keys))
            continue

        if a.description == 'Particles':
            line = a.keys[0]
            break
    else:
        for k in ('SPEFit2','MuonLlhFit','PoleMuonLlhFit','LineFit','PoleLineFit'):
            if k in frame:
                line = k
                break
        else:
            raise Exception('Cannot determine track. Please select one with the Particles artist')

    if not line:
        log = "Misconfigured Particle artist. Please remove it and try again."
        logging.log_error(log)
        raise Exception(log)

    # get pulses key
    bubbles = None
    for a in gl.scenario.getArtists():
        if not a.isActive:
            logging.log_warn("Ignoring inactive Artist {} with keys {}".format(a.description,a.keys))
            continue

        if a.description == 'Bubbles':
            bubbles = a.keys[1]
            break
    else:
        for k in ('SRTInIcePulses','SRTOfflinePulses','InIcePulses','OfflinePulses'):
            if k in frame:
                bubbles = k
                break
        else:
            raise Exception('Cannot determine pulses. Please select one with the Bubbles artist')

    if not bubbles:
        log = "Misconfigured Bubble artist. Please remove it and try again."
        logging.log_error(log)
        raise Exception(log)

    geometry=frame["I3Geometry"]
    track=frame[line]
    pulses=dataclasses.I3RecoPulseSeriesMap.from_frame(frame,bubbles)
    cog=dataclasses.I3Position(0,0,0)
    nchan=0
    for dom in pulses.keys():
        pos=geometry.omgeo[dom].position
        cog+=pos
        nchan+=1

    cog*=1./nchan
    cameraLoc=dataclasses.I3Position(gl.cameraLoc.x,gl.cameraLoc.y,gl.cameraLoc.z)
    pivot=cog
    center=dataclasses.I3Position(0,0,cog.z)

    #compute the view direction vector which is perpendicular to both the
    #reconstructed track and the vertical, and which looks into the detector
    #from whichever side is closer to the cog of the pulses
    # If this direction happens to be undefined, default to the cog-to-center line
    if not math.isnan(track.dir.azimuth) and (math.sin(track.dir.zenith) != 0.):
        x = -track.dir.y
        y = track.dir.x
    else:
        direction = center-cog
        x = direction.x
        y = direction.y

    viewDir=dataclasses.I3Direction(x,y,0)
    if viewDir*(center-cog)>0:
        viewDir=-viewDir

    cameraLoc=1600*viewDir
    cameraLoc.z = 9e2

    x = 25
    y = 20
    if cameraLoc.y < -500:
        y += 20
    elif cameraLoc.y < 0:
        y += 10
    x += cameraLoc.x/100*(-1)
    # Quadrant based shift
    if cameraLoc.x > 0 and cameraLoc.y > 0:
        x += 10
        y += 10
    elif cameraLoc.x < 0 and cameraLoc.y > 0:
        pass
    elif cameraLoc.x < 0 and cameraLoc.y < 0:
        x -= 10
    else:
        x -= 10
    if abs(math.sqrt(cameraLoc.x**2)-math.sqrt(cameraLoc.y**2)) < 800:
        if cameraLoc.x > 0 and cameraLoc.y > 0:
            x -= 10
            y += 10
        elif cameraLoc.x < 0 and cameraLoc.y > 0:
            x += 10
            y += 10
        elif cameraLoc.x < 0 and cameraLoc.y < 0:
            x += 10
            y -= 10
        else:
            x -= 10
            y -= 10

    pivot = (x, y, -63.2992)
    location = (cameraLoc)
    return (pivot, location)

def colorbar_size(width, scale):
    # height:width ratio is 1:10 for a scale factor of 1
    w = width
    h = width/10.*scale
    return (w, h)

def make_colorbar(filename, timerange, width, scale):
    from matplotlib.colorbar import ColorbarBase
    from matplotlib.colors import Normalize, LinearSegmentedColormap, hsv_to_rgb
    from matplotlib.cm import jet as cmap
    import numpy
    import pylab

    ref_dpi = pylab.rcParamsDefault["savefig.dpi"]
    rendering_dpi = float(ref_dpi*scale)
    w_inch = width/ref_dpi
    w, h = colorbar_size(w_inch, scale)

    h_pt = 72*h
    pylab.rc('font', family='serif', size=h_pt*0.25)

    fig = pylab.figure(figsize=(w, h), dpi=rendering_dpi)
    fig.subplots_adjust(left=5e-2, right=1-5e-2, bottom=0.75)
    ax = pylab.gca()

    timerange = timerange/1000.0
    intervals = int(round(9/scale))
    time_inc = timerange/(intervals-1)
    ticks = [round(time_inc*x,1) for x in range(intervals)]
    norm = Normalize(-0.25*time_inc, timerange+time_inc*0.25)
    
    N = 256*4
    hsv = numpy.vstack((numpy.linspace(0, 0.7, N), numpy.ones(N), numpy.ones(N))).T
    cmap = LinearSegmentedColormap.from_list('steamshovel', list(hsv_to_rgb(hsv.reshape(N,1,3)).squeeze()), N)

    cb = ColorbarBase(ax, cmap=cmap, norm=norm, orientation='horizontal')
    cb.set_label('Time [microseconds]')
    cb.set_ticks(ticks)
    pylab.savefig(filename)

def stitch(main, thumbs, colorbar, filename, width, height):
    y_offset_main = 0
    y_offset_colorbar = main.height
    if thumbs is not None:
        y_offset_main = thumbs[0].height
        y_offset_colorbar += y_offset_main
    if colorbar is not None:
        colorbar_padding = colorbar.height // 10
        y_offset_colorbar += colorbar_padding

    img = Image.new('RGB', (width, height), color=main.getpixel((0,0)))

    # add thumbs
    dx = 0
    if thumbs is not None:
        for thumb in thumbs:
            targetbox = (dx, 0, dx+thumb.width, thumb.height)
            img.paste(thumb, targetbox)
            dx += thumb.width

    # add main
    img.paste(main, (0, y_offset_main, img.width, y_offset_main+main.height))

    # add colorbar
    if colorbar is not None:
        centered_margin = (img.width - colorbar.width) // 2
        x0 = centered_margin
        y0 = y_offset_colorbar
        img.paste(colorbar, (x0, y0, x0+colorbar.width, y0+colorbar.height))

    img.save(filename, 'PNG')

def get_projection(filename, frame=None, include_xyz=True, include_colorscale=True,
                   width=1000, height=1000, dpi=0, scale=1., gamma=1.):
    """
    Get the 2d projection of the event, for use in publications.
    The final image has a (more or less) fixed aspect ratio,
    so width and height only define the maximum dimensions.

    Args:
        filename: filename to write to
        frame: I3Frame, defaults to None in which case the currently selected frame is used
        width: width of the final image's bounding box
        height: width of the final image's bounding box
        dpi: dpi of image
        scale: scale of image
        gamma: gamma of image
    """
    import __main__
    window = __main__.window
    if frame is None:
        frame  = __main__.frame
    tmpdir = tempfile.mkdtemp()
    try:
        basename = os.path.join(tmpdir,'img')
        mainname = basename+'_main.png'
        colorname = os.path.join(tmpdir,'color.png')
        thumbname = basename+'_{}.png'
        thumb_settings = [ # (name, location, up_direction)
            ('top', (0,0,1.5e3), (0,1,0)),
            ('x', (1.5e3,0,0), (0,0,1)),
            ('y', (0,-1.5e3,0), (0,0,1))
        ]

        # Determine image ratio and size within bounding box
        img_ratio = 1.

        if include_xyz:
            img_ratio += 1./len(thumb_settings)

        if include_colorscale:
            timerange = window.timeline.maxTime - window.timeline.minTime
            cb_w, cb_h = colorbar_size(1., scale)
            img_ratio += cb_h

        if float(height)/float(width) > img_ratio:
            height = int(width*img_ratio)
        else:
            width = int(height/img_ratio)

        # Render and stitch required parts
        with ProjectionRenderer(window.gl, dpi, scale, gamma) as pr:
            # render main
            pivot, loc = get_view(window.gl, frame)
            window.gl.perspectiveView = True
            pr.render(True, pivot, loc, (0,0,1), width, basename+"_main.png")

            if include_xyz:
                tres = width//len(thumb_settings)
                for name, loc, up in thumb_settings:
                    pr.render(False, (0,0,0), loc, up, tres, thumbname.format(name))

        thumbs = None
        if include_xyz:
            thumbs = [Image.open(thumbname.format(tag)) for tag,l,u in thumb_settings]

        colorscale = None
        if include_colorscale:
            make_colorbar(colorname, timerange, width, scale)
            colorscale = Image.open(colorname)

        main = Image.open(mainname)
        stitch(main, thumbs, colorscale, filename, width, height)
    finally:
        shutil.rmtree(tmpdir)
    
