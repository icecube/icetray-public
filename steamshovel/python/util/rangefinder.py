# -*- coding: utf-8 -*-
from __future__ import print_function
from icecube import icetray, dataclasses
import operator
import math
import numpy

def _i3map_values_iter(i3map, e1, e2):
    for dom, values in i3map:
        for v in values:
            yield (e1(v), e2(v))


def _particleTimeRange(obj):
    # Parameterize trajectory as vec(r) = vec(r0) + k*vec(d) with k = v*(t-t0)
    r0 = numpy.array([obj.pos.x, obj.pos.y, obj.pos.z])
    d = numpy.array([obj.dir.x, obj.dir.y, obj.dir.z])

    calc_pos = lambda k: k*d + r0
    calc_t = lambda k: k/obj.speed + obj.time
    calc_r = lambda pos: math.sqrt(pos[0]**2 + pos[1]**2)

    k_at_z = lambda z: (z-r0[2])/d[2]
    r_at_z = lambda z: calc_r(calc_pos(k_at_z(z)))

    # define cilinder around IceCube and calculate
    # punch-through times of the particle track.
    def line_circle_intersect(radius, r0, d):
      # determine roots of x² + y² = (k×d_xy + r0_xy)²
      a2 = 2*numpy.dot(d[:2], d[:2])
      b =  2*numpy.dot(r0[:2], d[:2])
      c = numpy.dot(r0[:2], r0[:2]) - radius**2
      try:
        sqrtD = math.sqrt(b**2 - 2*a2*c)
        return [(-b + sqrtD)/(a2), (-b - sqrtD)/(a2)]
      except ValueError:
        return []

    r_IC = 600
    z_IC = 500
    z_IT = 2000

    r_test_IT = r_IC+400
    r_test_IC = r_IC+150
    dz_test_IC = z_IC+150

    # Use 2D projections onto the XY-plane
    k_closest = -(numpy.dot(r0[:2], d[:2]))/numpy.dot(d[:2], d[:2])
    pos_closest = calc_pos(k_closest)
    if abs(pos_closest[2]) < dz_test_IC:
        min_r_IC = calc_r(pos_closest)
    else:
        min_r_IC = min(r_at_z(-dz_test_IC), r_at_z(dz_test_IC))

    passes_IT = r_at_z(z_IT) < r_test_IT
    passes_IC = min_r_IC < r_test_IC

    # Determine the along-track distance at which a plane perpendicular
    # to the track touches IceTop
    k_IT_crossing = k_at_z(z_IT)
    if abs(d[2]) <= 0.08: # altitude < 5°
        k_IT_in, k_IT_out = sorted(line_circle_intersect(r_IC, r0, d))
    elif abs(d[2]) < .996: # altitude < 85°
        dk = r_IC*math.sqrt(1-d[2]**2)
        k_IT_in = k_IT_crossing - dk
        k_IT_out = k_IT_crossing + dk
    else: # quasi down-going
        k_IT_in = k_IT_crossing
        k_IT_out = k_IT_crossing

    k_IC_z = sorted([k_at_z(dz_test_IC), k_at_z(-dz_test_IC)])
    k_IC_r = sorted(line_circle_intersect(r_test_IC, r0, d))
    k_IC_in = max(k_IC_z[0], k_IC_r[0])
    k_IC_out = min(k_IC_z[1], k_IC_r[1])

    # particle track time as function of position,
    # depending on wether the track passes (close to) IceCube or IceTop
    if passes_IC and passes_IT:
        k_in = min(k_IC_in, k_IT_in)
        k_out = k_IC_out # Ignores IT for up-going tracks through IceCube
    elif passes_IC:
        k_in, k_out = k_IC_in, k_IC_out
    elif passes_IT:
        k_in, k_out = k_IT_in, k_IT_out
    else:
        # shouldn't happen, but you never know with these arbitrary
        # volumes and surfaces
        k_in, k_out = k_IC_r

    start = calc_t(k_in)
    stop = calc_t(k_out)

    if math.isnan(start) or math.isnan(stop):
      return (0, 1)
    else:
      return (start, stop)


def getTimeRange(frame, key):
    '''
    Return the visible time range of frame[key], if one can be extracted.

    This is the backend of the python RangeFinder in TimelineWidget.  It returns a
    tuple of ints (start,end) incating the beginning and end of the given event
    according to the arbitrary frame object frame[key].  In general this is meant to
    detect the range of a RecoPulseSeriesMap or similar object.

    If no range can be gathered, this function returns None or throws an exception.
    Any python exception may be thrown; this simply indicates the key cannot be used
    for the given frame.

    Exotic types from unusual projects may be added here as well, but this module should
    not throw an exception when imported, so projects that aren't in offline-software
    should be loaded in a try block.
    '''

    obj = frame[key]

    # Handle I3EventHeader
    if isinstance(obj, dataclasses.I3EventHeader):
        return 0, obj.end_time - obj.start_time

    if isinstance(obj, dataclasses.I3TimeWindow):
        return obj.start, obj.stop

    # Handle I3Particle time-related position
    if isinstance(obj, dataclasses.I3Particle):
        try:
            return _particleTimeRange(obj)
        except Exception as e:
            print(e)

    # Handle maps of time-related data
    if not hasattr(obj, 'values'):
        # perhaps it's a mask
        obj = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, key)

    vals = obj.values()
    if len(vals) > 0:
        start = float('inf')
        end = 0
        # If the frame object has some values(), then find (min, max)
        # where max is either the maximum value.time, or the maximum
        # value[i].time for all i.  Similarly for min, mutatis mutandis.
        for x in vals:
            try:
                start = min(start, x.time)
                end = max(end, x.time)
            except AttributeError:
                for y in x:
                    start = min(start, y.time)
                    end = max(end, y.time)
        if start == float('inf'):
            start = 0
        duration = max(1, end - start) # make sure duration is always valid
        margin = 0.1
        return start - margin * duration, end + margin * duration


def getRangeHistogram(frame, key, timeRange):
    '''
    Given a frame, a key, and a tuple (start,end), attempt to create a histogram for the timeline widget.

    The returned histogram should be a dataclasses.I3VectorFloat with all values between 0 and 1.

    As with getTimeRange, this function may return None, or throw any exception, to indicate that
    producing a histogram with the given key is not possible.
    '''
    import numpy # numpy has to be available for this function to yield histograms

    obj = frame[key]

    # If a thing doesn't have a values attribute, we can't use it below;
    # but maybe it's a mask.  If not, this will throw and end the function.
    if not hasattr(obj, 'values'):
        obj = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, key)

    divs = min(256, int(timeRange[1] - timeRange[0]))

    if type(obj) == dataclasses.I3RecoPulseSeriesMap:
        # histogram the pulse times, weighting by charge
        e1 = operator.attrgetter('time')
        e2 = operator.attrgetter('charge')
    elif type(obj) == dataclasses.I3DOMLaunchSeriesMap:
        # histogram the launch times, with equal weighting
        e1 = operator.attrgetter('time')
        e2 = lambda x: 1
    else:
        return None

    hist_data, hist_weights = zip(*_i3map_values_iter(obj, e1, e2))
    hist, _ = numpy.histogram(
        hist_data, bins=divs, range=timeRange, weights=hist_weights)
    # normalize with logarithmic scaling
    hist = numpy.log1p(hist) / numpy.log(sum(hist))
    return dataclasses.I3VectorFloat(hist)
