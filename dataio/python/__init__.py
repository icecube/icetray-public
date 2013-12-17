from icecube import icetray, interfaces
from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

import sys
if sys.version_info[:2] >= (2,6):
	from icecube.dataio.I3FileStagerFile import I3FileStagerFile, GridFTPStager, SCPStager

def get_stagers(staging_directory=None):
	import os, pwd
	
	def try_to_make_scratch_dir(basename, fullname):
		if not os.path.isdir(basename):
			return False
		if os.path.isdir(fullname):
			return True

		try:
			os.mkdir(fullname)
		except OSError:
			return False

		return True

	if staging_directory is not None:
		pass
	# find a local staging directory
	elif "_CONDOR_SCRATCH_DIR" in os.environ:
		# works on condor (especially npx4)
		staging_directory = os.environ["_CONDOR_SCRATCH_DIR"]
	elif "TMPDIR" in os.environ:
		# works on some PBS/TORQUE nodes
		staging_directory = os.environ["TMPDIR"]
	else:
		# try some known locations on interactive nodes		
		current_username = pwd.getpwuid(os.getuid()).pw_name
		if try_to_make_scratch_dir('/scratch', '/scratch/'+current_username):
			staging_directory = '/scratch/'+current_username
		elif try_to_make_scratch_dir('/global/scratch', '/global/scratch/'+current_username):
			staging_directory = '/global/scratch/'+current_username
		# elif try_to_make_scratch_dir('/tmp', '/tmp/'+current_username):
		# 	staging_directory = '/tmp/'+current_username
		else:
			icetray.logging.log_fatal("Cannot find a suitable scratch directory on this machine.", unit="I3Reader")

	return [stager(staging_directory) for stager in (I3FileStagerFile, GridFTPStager, SCPStager)]

@icetray.traysegment_inherit('I3Reader')
def I3Reader(tray, name, **kwargs):
	"""Read an .i3 file. This supports remote files
	by specifying URLs and will stage them in an auto-configured
	local scratch directory. Using a URL like
	file://home/user/file.i3 will stage local files,
	using plain filenames will read the files directly.
	"""

	import os, pwd
	from icecube import icetray, dataclasses

	tray.context['I3FileStager'] = I3FileStagerCollection(get_stagers())

	tray.AddModule('I3Reader', name, **kwargs)

del icetray, interfaces
