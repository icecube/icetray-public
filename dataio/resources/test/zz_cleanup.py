#!/usr/bin/env python
import os, sys
from glob import glob

for fname in (["alpha.i3", "alphabet.i3", "catted.i3.gz", "evens.i3", 
    "filtered.i3", "ints.i3", "foo.i3", "noinfo.i3", "test.i3", "tmp2.i3", 
    "tmp.i3", "withnulls.i3", "orphanarium.i3", "pass1.i3", "one_event.i3",
    "hasmutineer.i3.gz", "split.gcd.i3", "testmultiD.*.i3", "testmulti.*.i3",
    "stillhasmutineer.i3.gz"] + 
                           glob("split.physics.*.i3")+ glob("pass1_*.i3.*")):
    if os.path.exists(fname):
        os.unlink(fname)

sys.exit(0)
