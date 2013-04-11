import copy
import os
import urlparse
import urllib2
import tempfile
import shutil

from icecube import icetray
from icecube.dataio import I3FileStager


class I3FileStagerFile(I3FileStager):
	file_counter = 0

	def __init__(self, local_scratch_dir):
		I3FileStager.__init__(self)

		self.local_scratch_dir = local_scratch_dir

		self.scratch_dir = None
		self.staged_files = set()

		if not os.path.isdir(self.local_scratch_dir):
			icetray.logging.log_fatal("The scratch directory %s does not exist!" % self.local_scratch_dir, unit="I3FileStagerFile")
	
		self.scratch_dir = tempfile.mkdtemp(dir=self.local_scratch_dir)

		icetray.logging.log_info("Created scratch directory %s" % self.scratch_dir, unit="I3FileStagerFile")

	def __del__(self):
		if self.staged_files is not None and len(self.staged_files)>0:
			icetray.logging.log_info("Cleaning up staged files..", unit="I3FileStagerFile")
			for staged_file in copy.copy(self.staged_files):
				self.Cleanup(staged_file)

		if self.scratch_dir is not None:
			icetray.logging.log_info("Removing scratch directory %s" % self.scratch_dir, unit="I3FileStagerFile")
			try:
				os.rmdir(self.scratch_dir)
			except OSError, e:
				print e

	def Schemes(self):
		# we handle "file://" URLs
		return ["http", "https", "ftp", "file"]

	def WillReadLater(self, url):
		# not used for this implementation, this will
		# just block while copying files in StageFile()
		pass

	def StageFile(self, url):
		# parse the URL
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme
		if parsed_url[0] not in self.Schemes():
			icetray.logging.log_fatal("Cannot handle URL scheme \"%s\": %s" % (parsed_url[0], url), unit="I3FileStagerFile")
		input_path = parsed_url[2]

		# parse the input filename
		input_dir, input_file = os.path.split(input_path)
		input_filebase, input_fileext = os.path.splitext(input_file)

		# support extensions like ".i3.bz2"
		if input_fileext in [".bz2", ".gz", ".xz"]:
			input_filebase2, input_fileext2 = os.path.splitext(input_filebase)
			input_filebase = input_filebase2
			input_fileext = input_fileext2+input_fileext

		output_file = input_filebase + ('_%06u' % I3FileStagerFile.file_counter) + input_fileext
		I3FileStagerFile.file_counter += 1

		output_path = self.scratch_dir + '/' + output_file

		self.staged_files.add(output_path)

		# copy the file
		if parsed_url[0] == "file":
			icetray.logging.log_info("Copying file %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
			shutil.copyfile(input_path, output_path)
			icetray.logging.log_info("File copied: %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
		else:
			icetray.logging.log_info("Downloading %s to %s" % (url, output_path), unit="I3FileStagerFile")

			try:
				f = urllib2.urlopen(url)
				data = f.read()
				with open(output_path, "wb") as code:
					code.write(data)
				f.close()

				icetray.logging.log_info("Download finished: %s to %s" % (url, output_path), unit="I3FileStagerFile")
			except urllib2.HTTPError, e:
				icetray.logging.log_fatal("Download error: %s" % str(e), unit="I3FileStagerFile")
				pass

		return output_path

	def Cleanup(self, filename):
		if filename not in self.staged_files:
			icetray.logging.log_error("File was never staged: %s" % filename, unit="I3FileStagerFile")
			return

		try:
			icetray.logging.log_info("Removing file %s" % filename, unit="I3FileStagerFile")
			os.remove(filename)
		except OSError, e:
			print e

		self.staged_files.remove(filename)



