#!/usr/bin/env python

# Test reading a zoo of archive/compression formats, courtesy of libarchive
# 
# Archive formats: tar, pax, cpio
# Compression formats: gzip, bzip2, lzma, xz
#

import os, sys

if not 'I3_TESTDATA' in os.environ:
	print("I3_TESTDATA is not set!")
	sys.exit(1)
	
fname = os.path.join(os.environ['I3_TESTDATA'], 'dataio/serialization/r51782/I3DOMLaunchSeriesMap.i3')

if not os.path.exists(fname):
	print("Can't find test file '%s'; skipping archive-reading tests." % fname)
	sys.exit(0)

from icecube import icetray, dataio

compressors = [ ('gz', 'gzip'), ('bz2', 'bzip2'), ('lzma', 'lzma'), ('xz', 'xz')]
archivers = [ ('tar', 'tar cf - %s'), ('pax', 'pax -w %s'), ('cpio', 'echo %s | cpio -o' )]

def test_read(filename):
	f = dataio.I3File(filename)
	fr = f.pop_frame()
	if list(fr.keys()) != ['object']:
		printf("Error reading '%s'!" % filename)
		sys.exit(1)

# No archiving/compression
if os.system('cp %s %s' % (fname, os.path.basename(fname))) != 0:
	print('Error copying input file')
archives = [os.path.basename(fname)]
outfiles = [os.path.basename(fname)]

# Try various archivers
for ar, ar_args in archivers:
	if os.system('which %s > /dev/null' % ar) != 0:
		continue
	outfile = "%s.%s" % (os.path.basename(fname), ar)
	cmd = "%s > %s" % (ar_args % fname, outfile)
	if os.system(cmd) == 0:
		archives += [outfile]
		outfiles += [outfile]
	else:
		print('Skipping %s archives due to creation failure.' % ar)
		os.unlink(outfile)

# Compress all combinations of archivers
for comp, comp_cmd in compressors:
	if comp_cmd is not None and os.system('which %s > /dev/null' % comp_cmd) != 0:
		# skip compressor if the binary can't be found
		continue
		
	for file in archives:
		outfile = "%s.%s" % (file, comp)
		cmd = "%s %s -c > %s" % (comp_cmd, file, outfile)

		if os.system(cmd) == 0:
			outfiles += [outfile]
		else:
			print('Failure during %s compression of %s' % (comp, file))
			os.unlink(outfile)

for file in outfiles:
	print(file)
	test_read(file)
	os.unlink(file)
