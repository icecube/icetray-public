import copy
import os, sys

if sys.version_info[0] >= 3:
	import urllib.parse as urlparse
	from urllib.error import HTTPError
	from urllib.request import urlopen, Request
else:
	import urlparse
	from urllib2 import HTTPError
	from urllib2 import urlopen, Request
import tempfile
import shutil
import subprocess

from icecube import icetray
from icecube.dataio import I3FileStager

class AbstractFileStager(I3FileStager):
	def __init__(self, local_scratch_dir):
		I3FileStager.__init__(self)
		self.staged_files = set()
		self.destinations = dict()
		self.local_scratch_dir = local_scratch_dir

		if not os.path.isdir(self.local_scratch_dir):
			icetray.logging.log_fatal("The scratch directory %s does not exist!" % self.local_scratch_dir, unit="I3FileStagerFile")
	
		self.scratch_dir = tempfile.mkdtemp(dir=self.local_scratch_dir)

		icetray.logging.log_info("Created scratch directory %s" % self.scratch_dir, unit="AbstractFileStager")
	
	def __del__(self):
		if self.staged_files is not None and len(self.staged_files)>0:
			icetray.logging.log_info("Cleaning up staged files..", unit="I3FileStagerFile")
			for staged_file in copy.copy(self.staged_files):
				self.Cleanup(staged_file)

		if self.scratch_dir is not None:
			icetray.logging.log_info("Removing scratch directory %s" % self.scratch_dir, unit="I3FileStagerFile")
			try:
				os.rmdir(self.scratch_dir)
			except OSError as e:
				icetray.logging.log_error("Cleanup error: %s" % str(e), unit="I3FileStagerFile")

	def WillReadLater(self, url):
		# not used for this implementation, this will
		# just block while copying files in StageFile()
		pass
	
	def WillWrite(self, url):
		self.check_scheme(url)
		if url in self.destinations:
			raise KeyError("Duplicate destination %s" % url)
		fname = self.generate_local_name(url)
		
		self.staged_files.add(fname)
		self.destinations[url] = fname
		
		return fname
	
	def Cleanup(self, filename):
		if filename not in self.staged_files:
			return

		try:
			icetray.logging.log_info("Removing file %s" % filename, unit="I3FileStagerFile")
			os.remove(filename)
		except OSError as e:
			icetray.logging.log_error("Cleanup error: %s" % str(e), unit="I3FileStagerFile")

		self.staged_files.remove(filename)

	def check_scheme(self, url):
		pass

	def generate_local_name(self, url):
		# parse the URL
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme
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

		output_path = self.scratch_dir + '/' + output_file
		
		return output_path

class I3FileStagerFile(AbstractFileStager):
	file_counter = 0

	def __init__(self, local_scratch_dir, blocksize=2**16):
		AbstractFileStager.__init__(self, local_scratch_dir)

		self.blocksize = blocksize

	def ReadSchemes(self):
		# we handle "file://" URLs
		schemes = ["http", "https", "ftp", "file"]
		return schemes
	
	def WriteSchemes(self):
		# we can't actually write anything
		return []

	def StageFileIn(self, url):
		# parse the URL
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme
		output_path = self.generate_local_name(url)

		# copy the file
		if parsed_url.scheme == "file":
			input_path = parsed_url.path
			icetray.logging.log_info("Copying file %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
			shutil.copyfile(input_path, output_path)
			icetray.logging.log_info("File copied: %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
		else:
			icetray.logging.log_info("Downloading %s to %s" % (url, output_path), unit="I3FileStagerFile")

			try:
				output_file = open(output_path, "wb")
				f = urlopen(url)
				
				while True:
					block = f.read(self.blocksize)
					output_file.write(block)
					if len(block) < self.blocksize:
						break

				icetray.logging.log_info("Download finished: %s to %s" % (url, output_path), unit="I3FileStagerFile")
			except Exception as e:
				if os.path.exists(output_path):
					os.remove(output_path)
				icetray.logging.log_fatal("Downloading %s: %s" % (url, str(e)), unit="I3FileStagerFile")
			finally:
				f.close()
				output_file.close()
		
		I3FileStagerFile.file_counter += 1
		self.staged_files.add(output_path)

		return output_path

	def StageFileOut(self, url):
		if not url in self.destinations:
			raise KeyError("%s was never registered as a destination")
		local_path = self.destinations[url]
		
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme
		if parsed_url[0] not in self.Schemes():
			icetray.logging.log_fatal("Cannot handle URL scheme \"%s\": %s" % (parsed_url[0], url), unit="I3FileStagerFile")

		# copy the file
		if parsed_url[0] == "file":
			output_path = parsed_url[2]
			icetray.logging.log_info("Copying file %s to %s" % (local_path, output_path), unit="I3FileStagerFile")
			shutil.copyfile(local_path, output_path)
			icetray.logging.log_info("File copied: %s to %s" % (local_path, output_path), unit="I3FileStagerFile")
		else:
			
			if not parsed_url.scheme in ('ftp', 'gsiftp'):
				icetray.log_fatal("Can't upload to %s" % url.scheme)
			
			icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="I3FileStagerFile")
			proc = subprocess.Popen(['globus-url-copy', '-nodcau', 'file://'+os.path.abspath(local_path), url], stderr=subprocess.PIPE)
			stdout, stderr = proc.communicate()
			if proc.returncode != 0:
				icetray.logging.log_error("globus-url-copy failed: "+(stderr.strip()), unit="I3FileStagerFile")
			else:
				icetray.logging.log_info("Upload finished: %s to %s" % (local_path, url), unit="I3FileStagerFile")
			
		self.Cleanup(local_path)
		
class GridFTPStager(AbstractFileStager):
	
	def __init__(self, local_scratch_dir):
		super(type(self), self).__init__(local_scratch_dir)
	
	def ReadSchemes(self):
		return ['ftp', 'gsiftp']
	
	def WriteSchemes(self):
		return ['ftp', 'gsiftp']
	
	def StageFileIn(self, url):
		self.check_scheme(url)
		
		local_path = self.generate_local_name(url)
		
		icetray.logging.log_info("Downloading %s to %s" % (url, local_path), unit="GridFTPStager")
		proc = subprocess.Popen(['globus-url-copy', '-nodcau', url, 'file://'+os.path.abspath(local_path)], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_error("globus-url-copy failed: "+(stderr.strip()), unit="GridFTPStager")
		else:
			icetray.logging.log_info("Download finished: %s to %s" % (url, local_path), unit="GridFTPStager")
		
		self.staged_files.add(local_path)
		return local_path
		
	def StageFileOut(self, url):
		if not url in self.destinations:
			raise KeyError("%s was never registered as a destination")
		local_path = self.destinations[url]
		
		icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="GridFTPStager")
		proc = subprocess.Popen(['globus-url-copy', '-nodcau', 'file://'+os.path.abspath(local_path), url], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_error("globus-url-copy failed: "+(stderr.strip()), unit="GridFTPStager")
		else:
			icetray.logging.log_info("Upload finished: %s to %s" % (local_path, url), unit="GridFTPStager")
		
		self.Cleanup(local_path)

class SCPStager(AbstractFileStager):
	
	def __init__(self, local_scratch_dir):
		super(type(self), self).__init__(local_scratch_dir)
	
	def ReadSchemes(self):
		return ['scp']
	
	def WriteSchemes(self):
		return ['scp']
	
	def StageFileIn(self, url):
		
		local_path = self.generate_local_name(url)
		
		parsed = urlparse.urlparse(url)
		remote_spec = parsed.hostname+":"+parsed.path
		icetray.logging.log_info("Downloading %s to %s" % (url, local_path), unit="SCPFTPStager")
		proc = subprocess.Popen(['scp', remote_spec, os.path.abspath(local_path)], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_error("scp failed: "+(stderr.strip()), unit="SCPFTPStager")
		else:
			icetray.logging.log_info("Download finished: %s to %s" % (url, local_path), unit="SCPFTPStager")
		
		self.staged_files.add(local_path)
		return local_path
		
	def StageFileOut(self, url):
		if not url in self.destinations:
			raise KeyError("%s was never registered as a destination")
		local_path = self.destinations[url]
		
		parsed = urlparse.urlparse(url)
		remote_spec = parsed.hostname+":"+parsed.path
		icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="GridFTPStager")
		proc = subprocess.Popen(['scp', os.path.abspath(local_path), remote_spec], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_error("scp failed: "+(stderr.strip()), unit="GridFTPStager")
		else:
			icetray.logging.log_info("Upload finished: %s to %s" % (local_path, url), unit="GridFTPStager")
		
		self.Cleanup(local_path)