#!/usr/bin/env python

import sys, pprint, os, os.path, shutil

srcfile = sys.argv[1]
srcdir = os.path.dirname(srcfile)
dstdir = sys.argv[2]

realpath = os.path.realpath(srcfile)
print("%s => %s" % (srcfile, dstdir))

print("realpath: %s" % realpath)

linkedfiles = []

for f in os.listdir(srcdir):
    real = os.path.realpath(os.path.join(srcdir, f))
    if real == realpath:
        linkedfiles += [f]

linkedfiles.sort()
firstone = linkedfiles.pop()

dst_realfile = os.path.join(dstdir, firstone)

if not os.path.exists(dstdir):
    os.makedirs(dstdir)

print("cp %s %s" % (realpath, dst_realfile))

if os.path.exists(dst_realfile):
    os.unlink(dst_realfile)

shutil.copy2(realpath, dst_realfile)

for f in linkedfiles:
    to_create = os.path.join(dstdir, f)
    print("symlink %s %s" % (firstone, to_create))
    if os.path.exists(to_create):
        os.unlink(to_create)
    os.symlink(firstone, to_create)

