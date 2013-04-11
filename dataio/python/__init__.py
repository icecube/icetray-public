from icecube import icetray, interfaces
from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from I3FileStagerFile import I3FileStagerFile

@icetray.traysegment_inherit('I3Reader', removeopts=['FileStagerList'])
def I3Reader(tray, name, **kwargs):
	"""Read an .i3 file. This supports remote files
	by specifying URLs and will stage them in an auto-configured
	local scratch directory. Using a URL like
	file://home/user/file.i3 will stage local files,
	using plain filenames will read the files directly.
	"""

	import os, pwd
	from icecube import icetray, dataclasses

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

	# find a local staging directory
	if "_CONDOR_SCRATCH_DIR" in os.environ:
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

	stager = I3FileStagerFile(staging_directory)
	tray.AddModule('I3Reader', name,
		FileStagerList=[stager],
		**kwargs)

del icetray, interfaces
