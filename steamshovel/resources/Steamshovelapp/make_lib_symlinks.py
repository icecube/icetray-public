#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys

for x in [os.path.realpath(_) for _ in sys.argv[1:]]:
    os.chdir( os.path.dirname(x) )
    parts = x.split('.')
    n = len(parts)
    keep = []  # type: list[str]
    for i in range(n):
        part = parts[n - 1 - i]
        if part.isdigit():
            sl = '.'.join(parts[:n - 1 - i] + keep)
            if os.path.exists(sl):
                if os.path.islink(sl):
                    os.unlink(sl)
                else:
                    continue
            os.symlink(x, sl)
        else:
            keep.append(part)
