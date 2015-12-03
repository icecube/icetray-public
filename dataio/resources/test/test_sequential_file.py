#!/usr/bin/env python

from icecube import icetray, dataio

import os
import tempfile
import shutil

try:
    tmpdir = tempfile.mkdtemp()

    name = os.path.join(tmpdir,'f1.i3')
    f = dataio.I3File(name,'w')
    f.push(icetray.I3Frame(icetray.I3Frame.Geometry))
    f.push(icetray.I3Frame(icetray.I3Frame.Calibration))
    f.push(icetray.I3Frame(icetray.I3Frame.DetectorStatus))
    f.push(icetray.I3Frame(icetray.I3Frame.DAQ))
    f.push(icetray.I3Frame(icetray.I3Frame.Physics))
    f.push(icetray.I3Frame(icetray.I3Frame.DAQ))
    f.push(icetray.I3Frame(icetray.I3Frame.Physics))
    f.close()

    # skip the first frame
    f = dataio.I3File(name,dataio.I3File.Mode.Reading,1)
    assert(f.pop_frame().Stop == icetray.I3Frame.Calibration)
    f.close()

    # test for error
    try:
        f = dataio.I3File(name,'r',100)
    except Exception:
        pass
    else:
        raise Exception('should have raised error for iterating past end of file')
    f.close()

    # open in closed mode
    f = dataio.I3File(name,'x')
    if f.more():
        raise Exception('should not be able to more() a closed file')

    # test pop_frame for a non-reading file
    try:
        f.pop_frame()
    except Exception:
        pass
    else:
        raise Exception('should not be able to pop_frame() on a non-reading file')

    # try push on a non-writing file
    f = dataio.I3File(name,'r')
    try:
        f.push(icetray.I3Frame(icetray.I3Frame.DAQ))
    except Exception:
        pass
    else:
        raise Exception('should not be able to push() to a non-writing file')
    
    # get mixed frames
    f.pop_frame()
    f.pop_frame()
    f.pop_frame()
    f.pop_frame()
    fr = f.get_mixed_frames()
    if len(fr) != 3:
        raise Exception('not 3 mixed frames')
    if fr[0].Stop != icetray.I3Frame.Geometry:
        raise Exception('no mixed Geometry frame')
    if fr[1].Stop != icetray.I3Frame.Calibration:
        raise Exception('no mixed Calibration frame')
    if fr[2].Stop != icetray.I3Frame.DetectorStatus:
        raise Exception('no mixed DetectorStatus frame')
    f.close()

    f = dataio.I3File(name,'r')
    iter = f.__iter__()
    iter2 = iter.__iter__()
    f.close()

except:
    shutil.rmtree(tmpdir)
    raise
