#!/usr/bin/env python
# -*- coding: utf-8 -*-
from icecube.shovelio import raw
from icecube import icetray
from icecube.icetray.I3Test import ENSURE
import os
import random
random.seed(1)

# icetray.set_log_level_for_unit("shovelio::raw", icetray.I3LogLevel.LOG_TRACE)

seekable_file = os.path.expandvars("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")
multipass_file = os.path.expandvars("$I3_TESTDATA/nugen_numu_ic80_dc6.002488.000000.processed.i3.gz")

sframe_list = []
for filename in (seekable_file, multipass_file):

    # check that raw.FileGroup with one file behaves like raw.File

    pos_list = []
    size_list = []
    for cls in (raw.File, raw.FileGroup):
        f = cls(filename)
        pos = []
        while 1:
            pos.append(f.current_stream_pos)
            try:
                frame = f.next(True)
            except StopIteration:
                pos.pop()
                break
        pos_list.append(pos)
        size_list.append(f.size)

    ENSURE(pos_list[1] == pos_list[0], "positions differ from raw::File")
    ENSURE(size_list[1] == size_list[0], "size differs from raw::File")

    f = raw.File(filename)
    f2 = raw.FileGroup(filename)

    for p in pos:
        frame = f.at_stream_pos(p)
        frame2 = f2.at_stream_pos(p)
        ENSURE(str(frame) == str(frame2), "frames differ at position {}".format(p))

    # check that raw.FileGroup with two files behaves properly

    pos1 = pos[:]
    for p in pos[:]:
        pos1.append(size_list[0] + p)

    for fast in (0, 1):
        pos2 = []
        fg = raw.FileGroup(filename, filename)
        while 1:
            pos2.append(fg.current_stream_pos)
            try:
                fg.next(fast)
            except StopIteration:
                pos2.pop()
                break

        ENSURE(pos1 == pos2, "raw.FileGroup(myfile, myfile) has wrong stream positions")

    ENSURE(f.size * 2 == fg.size,
           "raw.FileGroup(myfile, myfile) is not twice the size of raw.File(myfile): {} vs {}"\
           .format(fg.size, f.size))

    f = raw.File(filename)
    fg = raw.FileGroup(filename, filename)

    random.shuffle(pos1)
    for p in pos1:
        frame_ref = f.at_stream_pos(p % f.size)
        frame = fg.at_stream_pos(p)
        ENSURE(str(frame) == str(frame_ref),
               "raw.FileGroup vs raw.File, frames differ at position {}".format(p))
