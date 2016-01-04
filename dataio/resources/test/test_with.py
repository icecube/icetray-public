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

    n_frames = 0
    with dataio.I3File(name) as f:
        while f.more():
            n_frames += 1
            f.pop_frame()
    if n_frames != 7:
        raise Exception('wrong number of frames')

    # now test an already open file
    n_frames = 0
    f1 = dataio.I3File(name)
    with f1 as f:
        while f.more():
            n_frames += 1
            f.pop_frame()
    if n_frames != 7:
        raise Exception('wrong number of frames')

    try:
        f1.pop_frame()
    except RuntimeError:
        pass
    else:
        raise Exception('pop_frame on closed file worked')

finally:
    shutil.rmtree(tmpdir)
