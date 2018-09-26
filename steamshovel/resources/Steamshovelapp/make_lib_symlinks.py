#!/usr/bin/env python2.7
import os
import sys

for x in [os.path.realpath(_) for _ in sys.argv[1:]]:
    os.chdir( os.path.dirname(x) )
    parts = x.split('.')
    n = len(parts)
    keep = []
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
