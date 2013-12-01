#!/usr/bin/env python

import sys
if sys.version_info[:2] < (2,6):
	sys.exit(0)

from icecube.dataio import I3FileStagerFile

from icecube import icetray
import os
icetray.logging.I3Logger.global_logger = icetray.I3NullLogger()

def test_scratchdir():
	stager = I3FileStagerFile('.')
	scratch_dir = stager.scratch_dir
	assert(os.path.isdir(scratch_dir))
	del stager
	assert(not os.path.isdir(scratch_dir))

def _test_stage(url):
	stager = I3FileStagerFile('.')
	local_fname = stager.StageFile(url)
	assert(len(stager.staged_files) == 1)
	assert(local_fname == iter(stager.staged_files).next())
	assert(os.path.exists(local_fname))
	assert(os.stat(local_fname).st_size > 100)

def test_http():
	_test_stage("http://code.icecube.wisc.edu/tools/clsim/MD5SUMS")

def test_https():
	_test_stage("https://icecube.wisc.edu/index.html")

def test_ftp():
	_test_stage("ftp://ftp.freebsd.org/pub/FreeBSD/README.TXT")

def test_gridftp():
	if not 'gsiftp' in I3FileStagerFile('.').Schemes():
		return
	_test_stage("gsiftp://gridftp.icecube.wisc.edu/data/sim/sim-new/downloads/spline-tables/README")
	# ensure that errors are properly reported
	try:
		_test_stage("gsiftp://gridftp.icecube.wisc.edu/data/sim/sim-new/downloads/spline-tables/READMEY")
		raise AssertionError("This file does not actually exist.")
	except RuntimeError:
		pass
	except Exception as e:
		raise e

def test_file():
	_test_stage(os.path.expandvars("file://$I3_BUILD/env-shell.sh"))
	
def test_file_implicit():
	_test_stage(os.path.expandvars("$I3_BUILD/env-shell.sh"))

if __name__ == "__main__":
	for k, v in locals().items():
		if k.startswith("test") and hasattr(v, "__call__"):
			v()
