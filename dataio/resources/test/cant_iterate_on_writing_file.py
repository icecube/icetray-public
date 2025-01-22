#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Ensure that if an I3File is opened for writing it won't give you an iterator.
#
from icecube import dataio
import sys, os

fn = "alphabet2.i3"
i3f = dataio.I3File(fn, 'w')

try:
    for frame in i3f:
        print(frame)
except RuntimeError:
    print("ok, that threw as expected")
    sys.exit(0)
except Exception as e:
    print("FAIL: that threw with an unexpected Exception")
    raise(e)
else:
    print("FAIL: that should have thrown")
    sys.exit(1)
finally:
    if os.path.exists(fn):
        os.unlink(fn)
