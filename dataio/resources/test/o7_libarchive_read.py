#!/usr/bin/env python

# Test reading a zoo of archive/compression formats, courtesy of libarchive
# 
# Archive formats: tar, pax, cpio
# Compression formats: gzip, bzip2, lzma, xz
#

import os, sys

if not 'I3_PORTS' in os.environ:
	print "I3_PORTS is not set!"
	sys.exit(1)
	
if not os.path.exists(os.path.join(os.environ['I3_PORTS'], 'bin/xz')):
	print "xz (and thus, libarchive) is not installed; skipping archive-reading tests."
	sys.exit(0)
	
fname = os.path.join(os.environ['I3_BUILD'], 'dataio/resources/data/serialization/r51782/I3DOMLaunchSeriesMap.i3')

if not os.path.exists(fname):
	print "Can't find test file '%s'; skipping archive-reading tests." % fname
        sys.exit(0)

from icecube import icetray, dataio

compressors = [ None, 'gz', 'bz2', 'lzma', 'xz']
archivers = [ (None, None), ('tar', 'tar cf - %s'), ('pax', 'pax -w %s'), ('cpio', 'echo %s | cpio -o' )]

def test_read(filename):
	f = dataio.I3File(filename)
	fr = f.pop_frame()
	if fr.keys() != ['object']:
		printf("Error reading '%s'!" % filename)
		sys.exit(1)

for ar, ar_args in archivers:
	for comp in compressors:
		if comp is not None and os.system('which %s > /dev/null' % comp) != 0:
			# skip compressor if the binary can't be found
			continue
		if ar is not None and os.system('which %s > /dev/null' % ar) != 0:
			continue
		
		if ar is None and comp is None:
			outfile = "%s.i3" % os.path.basename(fname)
			cmd = "cp %s %s" % (fname, outfile)
		elif ar is not None and comp is None:
			outfile = "%s.%s" % (os.path.basename(fname), ar)
			cmd = "%s > %s" % (ar_args % fname, outfile)
		elif ar is None and comp is not None:
			outfile = "%s.%s" % (os.path.basename(fname), comp)
			cmd = "%s %s -c > %s" % (comp, fname, outfile)
		else:
			outfile = "%s.%s.%s" % (os.path.basename(fname), ar, comp)
			cmd = "%s  | %s -c > %s" % (ar_args % fname, comp, outfile)

		os.system(cmd)
		test_read(outfile)
		os.unlink(outfile)
