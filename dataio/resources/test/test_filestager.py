#!/usr/bin/env python

import sys
if sys.version_info[:2] < (2,6):
	sys.exit(0)

from icecube.dataio import I3FileStagerFile

from icecube import icetray
import os
# icetray.logging.I3Logger.global_logger = icetray.I3NullLogger()
icetray.logging.set_level('TRACE')

def test_scratchdir(url=os.path.expandvars("file://$I3_BUILD/env-shell.sh")):
	stager = I3FileStagerFile('.')
	# directory is created lazily
	scratch_dir = stager.scratch_dir
	assert(scratch_dir is None)
	# now it should exist
	stager.GetReadablePath(url)
	scratch_dir = stager.scratch_dir
	assert(os.path.isdir(scratch_dir))
	del stager
	# now it should be gone
	assert(not os.path.isdir(scratch_dir))

def _test_stage(url, minsize=100):
	stager = I3FileStagerFile('.')
	local_fname = stager.GetReadablePath(url)
	assert(os.path.exists(str(local_fname)))
	assert(os.stat(str(local_fname)).st_size > minsize)
	local_fname = str(local_fname)
	# check that staged files are really deleted
	if stager.CanStageIn(url):
		assert(not os.path.exists(str(local_fname)))

def test_http():
	_test_stage("http://code.icecube.wisc.edu/tools/clsim/MD5SUMS")

def test_http_with_auth():
	_test_stage("http://icecube:skua@convey.icecube.wisc.edu/data/sim/sim-new/downloads/globus.tar.gz.md5sum", 32)

def test_https():
	_test_stage("https://icecube.wisc.edu/index.html")

def test_file():
	_test_stage(os.path.expandvars("file://$I3_BUILD/env-shell.sh"))
	
def test_file_implicit():
	_test_stage(os.path.expandvars("$I3_BUILD/env-shell.sh"))

if __name__ == "__main__":
	for k, v in list(locals().items()):
		if k.startswith("test") and hasattr(v, "__call__"):
			v()
