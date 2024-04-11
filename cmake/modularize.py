#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import shutil, sys, os, os.path
srcdir = sys.argv[1]
srcs = sys.argv[2:-1]
dst = sys.argv[-1]

print("%s => %s" % (srcs, dst))
try:
    shutil.rmtree(dst)
except:
    pass
os.mkdir(dst)

for src in srcs:
    shutil.move(os.path.join(srcdir, src), os.path.join(dst, src))


