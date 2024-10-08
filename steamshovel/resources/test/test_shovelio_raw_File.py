#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import random
import subprocess as subp
import sys

from icecube.shovelio import raw

random.seed(1)

# icetray.set_log_level_for_unit("shovelio::raw", icetray.I3LogLevel.LOG_TRACE)

uncompressed_file = os.path.expandvars("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")

if len(sys.argv) == 1:
    init_args = ((uncompressed_file,),
                 (uncompressed_file, raw.File.Type.MultiPass),
                 (uncompressed_file, raw.File.Type.SinglePass))

    pos_list = []
    size_list = []
    stream_list = []
    sframe_list = []
    for args in init_args:
        for fast in (0, 1):
            f = raw.File(*args)
            pos = []
            sframes = []
            streams = []
            while 1:
                pos.append(f.current_stream_pos)
                try:
                    frame = f.next(fast)
                    sframes.append(str(frame))
                    streams.append(str(frame.Stop))
                except StopIteration:
                    pos.pop()
                    break
                except RuntimeError:
                    pos.pop()
                    break
            pos_list.append(pos)
            size_list.append(f.size)
            sframe_list.append(sframes)
            stream_list.append(streams)

    assert pos_list[0] == pos_list[1], \
           "positions from Next(..., fast) differ between fast=True and fast=False for Seekable"

    assert pos_list[2] == pos_list[3], \
           "positions from Next(..., fast) differ between fast=True and fast=False for MultiPass"

    assert pos_list[4] == pos_list[2], \
           "positions from Next(..., True) with type=SinglePass differ from type=MultiPass"

    assert pos_list[5] == [], \
           "positions from Next(..., False) with type=SinpePass are not empty"

    for i, j in ((0, 1), (2, 3),
                 (0, 2), (0, 4),
                 (1, 3)):
        # this is awful, but more clear than an f-string or %-string
        assert stream_list[i] == stream_list[j], \
             "streams from {0}-Next(...) and {1}-Next(...) do not agree:\n{0}: {2}\n{1}: {3}" \
             .format(i, j, " ".join(stream_list[i]), " ".join(stream_list[j]))

    for i, j in ((0, 2), (0, 4)):
        # this is awful, but more clear than an f-string or %-string
        assert sframe_list[i] == sframe_list[j], \
             "frames from {0}-Next(...) and {1}-Next(...) do not agree:\n{0}: {2}\n{1}: {3}" \
             .format(i, j, "\n".join(sframe_list[i]), "\n".join(sframe_list[j]))

    actual_size = 615132
    assert size_list[0] == actual_size, f"size in raw.File 1 differs from {actual_size} (Seekable)"
    assert size_list[1] == actual_size, f"size in raw.File 2 differs from {actual_size} (Seekable)"

    actual_size = 8
    assert size_list[2] == actual_size, f"size in raw.File 3 differs from {actual_size} (MultiPass)"
    assert size_list[3] == actual_size, f"size in raw.File 4 differs from {actual_size} (MultiPass)"

    f1 = raw.File(uncompressed_file)
    f2 = raw.File(uncompressed_file, raw.File.Type.MultiPass)

    pairs = list(zip(pos_list[0], pos_list[2]))
    random.shuffle(pairs)

    for p1, p2 in pairs:
        frame1 = f1.at_stream_pos(p1)
        frame2 = f2.at_stream_pos(p2)
        s1 = str(frame1)
        s2 = str(frame2)
        assert s1 == s2, f"frames differ at positions ({p1}, {p2}):\n{s1}\n{s2}"

    ### test reading from pipe
    proc = subp.Popen((sys.argv[0], "testpipe"),
                      stdin=open(uncompressed_file, "rb"))  # noqa: SIM115
    proc.wait()
    sys.exit(proc.returncode)

elif sys.argv[1] == "testpipe":
    # this code is executed in the subprocess
    f = raw.File("-")
    f_ref = raw.File(uncompressed_file)
    sframes = []
    sframes_ref = []
    while 1:
        try:
            frame = f.next(False)
            sframes.append(str(frame))
        except StopIteration:
            break
    while 1:
        try:
            frame = f_ref.next(False)
            sframes_ref.append(str(frame))
        except StopIteration:
            break

    assert f.size == len(sframes), "testpipe: size is incorrect"

    for p in range(len(sframes)):
        assert sframes[p] == sframes_ref[p], \
               f"testpipe: frames do not match at pos={p}"
