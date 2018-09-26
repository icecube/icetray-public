from icecube.shovelart import *

from icecube import dataclasses
try: from icecube import simclasses
except: pass # don't complain if simclasses is not available


class Bubbles( PyArtist ):
    """
    Mimics the behavior of ChargeBubbles for keys that support it,
    and works with all kinds of containers of OMKeys.
    """

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            "colormap", I3TimeColorMap(),
            "scale", RangeSetting( 0.0, 100.0, 100, 10.0 ),
            "power", RangeSetting( 0.0, 0.5, 50, 0.15 ),
            "static", PyQColor.fromRgb( 255, 0, 255 ),
            "log10(delay/ns)", RangeSetting( 1.0, 5.0, 40, 5.0 ),
            "custom color window", ""
        ))

    numRequiredKeys = 2

    def isValidKey( self, frame, key_idx, key ):
        tn = frame.type_name(key)
        if key_idx == 0:
            return tn == "I3Geometry"
        if key_idx == 1:
            # Key may be any container of OMKeys or a mask
            return (("OMKey" in tn
                    and not "I3ParticleID" in tn )
                    or tn.endswith("MapMask")
                    or tn.endswith("MapUnion"))
        return False

    def create( self, frame, output ):
        scale = self.setting("scale")
        power = self.setting("power")
        colormap = self.setting("colormap")
        static_color = self.setting("static")
        delay = 10 ** self.setting("log10(delay/ns)")
        custom_time_range = get_custom_time_range(self.setting("custom color window"))

        # set by discovery later
        self.has_scale = None
        self.name_scale = None
        self.has_time = None

        geom_key, frame_key = self.keys()
        omgeo = frame[geom_key].omgeo
        fobj = frame[frame_key]

        if isinstance(fobj, (dataclasses.I3RecoPulseSeriesMapMask,
                             dataclasses.I3RecoPulseSeriesMapUnion)):
            fobj = fobj.apply(frame)

        if hasattr(fobj, "keys"):
            # an I3Map of OMKeys
            ommap = fobj
            for omkey, val in ommap:
                pos = omgeo[omkey].position
                sphere = output.addSphere( scale, pos )
                sphere.setSelectionContent( omkey )

                if hasattr(val, "__len__"):
                    # val is a series
                    series = val
                    if len(series) == 0: continue

                    self.discover_features( series[0] )

                    if self.has_time:
                        if delay < 1e5: # keep in sync with upper limit of RangeSetting
                            self.handle_integral_bubble_delay( output, sphere, series,
                                                               scale, power, colormap,
                                                               delay, custom_time_range )
                        else:
                            self.handle_integral_bubble( output, sphere, series,
                                                         scale, power, colormap,
                                                         custom_time_range )
                else:
                    self.discover_features( val )

                    if self.has_time:
                        self.handle_time_bubble( output, sphere, val,
                                                 scale, colormap,
                                                 custom_time_range )
                    else:
                        self.handle_const_bubble( sphere, static_color )

        elif hasattr(fobj, "__len__"):
            # a sequence of OMKeys
            for omkey in fobj:
                pos = omgeo[omkey].position
                sphere = output.addSphere( scale, pos )
                sphere.setSelectionContent( omkey )
                self.handle_const_bubble( sphere, static_color )

    def discover_features( self, obj ):
        # discover object properties
        if self.has_time is not None:
            return # already set

        self.has_time = hasattr(obj, "time")

        for self.name_scale in ("charge", "npe"):
            self.has_scale = hasattr(obj, self.name_scale)
            if self.has_scale:
                return # self.name_scale now also has correct value

    def handle_integral_bubble_delay( self, output, sphere, series,
                                      scale, power, colormap, delay,
                                      custom_time_range ):
        times = [x.time for x in series]
        sizefunc = StepFunctionFloat(0)
        chargemap = {}
        change_points = []
        for obj in series:
            t = obj.time
            if self.has_scale:
                chargemap[t] = getattr(obj, self.name_scale)
            else:
                chargemap[t] = 1.0
            change_points.append(t)
            change_points.append(t + delay)
        change_points.sort()
        for i,tc in enumerate(change_points):
            accum = 0.0
            j = i
            while j >= 0:
                t = change_points[j]
                # beware of round-off errors:
                # t <= (tc - delay) is not equivalent!
                if (t + delay) <= tc:
                    break
                if chargemap.has_key(t):
                    accum += chargemap[t]
                j -= 1
            size = scale * ( 0.2 * accum ) ** power
            sizefunc.add( size, tc )
        if custom_time_range:
            # prevent garbage collection of new TimeWindow
            output.custom_tw = TimeWindow( *custom_time_range )
            tw = output.custom_tw
        else:
            tw = output
        sphere.setColor( TimeWindowColor( tw, times, colormap ) )
        sphere.setSize( sizefunc )

    def handle_integral_bubble( self, output, sphere, series,
                                scale, power, colormap,
                                custom_time_range ):
        t0 = series[0].time
        sizefunc = StepFunctionFloat(0)
        accum = 0
        for obj in series:
            if self.has_scale:
                accum += getattr(obj, self.name_scale)
            else:
                accum += 1.0
            size = scale * ( 0.2 * accum ) ** power
            sizefunc.add( size, obj.time )
        if custom_time_range:
            # prevent garbage collection of new TimeWindow
            output.custom_tw = TimeWindow( *custom_time_range )
            tw = output.custom_tw
        else:
            tw = output
        sphere.setColor( TimeWindowColor( tw, t0, colormap ) )
        sphere.setSize( sizefunc )

    def handle_time_bubble( self, output, sphere, obj,
                            scale, colormap,
                            custom_time_range ):
        t0 = obj.time
        sizefunc = StepFunctionFloat(0)
        sizefunc.add( scale, t0 )
        if custom_time_range:
            # prevent garbage collection of new TimeWindow
            output.custom_tw = TimeWindow( *custom_time_range )
            tw = output.custom_tw
        else:
            tw = output
        sphere.setColor( TimeWindowColor( tw, t0, colormap ) )
        sphere.setSize( sizefunc )

    def handle_const_bubble( self, sphere, static_color ):
        sphere.setColor( static_color )


def get_custom_time_range(s):
    result = []
    for part in s.replace(","," ").split():
        if part:
            try: result.append(float(part))
            except: pass
    if (len(result) != 2 
        or result[0] >= result[1]):
        result = []
    return result
