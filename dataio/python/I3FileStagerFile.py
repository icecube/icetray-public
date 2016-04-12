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
import base64
import tempfile
import shutil
import subprocess

from icecube import icetray
from icecube.dataio import I3FileStager

class AbstractFileStager(I3FileStager):
	_local_scratch_dir = None
	def __init__(self):
		I3FileStager.__init__(self)
		self.scratch_dir = None
		self.file_counter = 0
	
	@staticmethod
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
	
	@classmethod
	def get_subclasses(cls):
		"""
		Assemble a list of all subclasses defined in the current module
		"""
		import inspect
		module = inspect.getmodule(cls)
		return [klass for name, klass in inspect.getmembers(module, inspect.isclass) if klass != cls and issubclass(klass, cls)]
	
	@classmethod
	def get_local_scratch_dir(cls):
		if cls._local_scratch_dir is not None:
			return cls._local_scratch_dir
		# find a local staging directory
		elif "_CONDOR_SCRATCH_DIR" in os.environ:
			# works on condor (especially npx4)
			staging_directory = os.environ["_CONDOR_SCRATCH_DIR"]
		elif "TMPDIR" in os.environ:
			# works on some PBS/TORQUE nodes
			staging_directory = os.environ["TMPDIR"]
		else:
			# try some known locations on interactive nodes
			import pwd
			current_username = pwd.getpwuid(os.getuid()).pw_name
			if cls.try_to_make_scratch_dir('/scratch', '/scratch/'+current_username):
				staging_directory = '/scratch/'+current_username
			elif cls.try_to_make_scratch_dir('/global/scratch', '/global/scratch/'+current_username):
				staging_directory = '/global/scratch/'+current_username
			else:
				staging_directory = os.getcwd()
				icetray.logging.log_info("Cannot find a suitable scratch directory on this machine; falling back to the current working directory (%s). If this is not what you want, set a different path with dataio.set_local_scratch_dir(path)." % staging_directory, unit="I3Reader")
		
		cls.set_local_scratch_dir(staging_directory)
		return cls._local_scratch_dir
	
	@classmethod
	def set_local_scratch_dir(cls, staging_directory):
		"""
		Set the directory to use for staging temporary files, overriding the
		default guesses.
		"""
		if not os.path.isdir(staging_directory):
			icetray.logging.log_fatal("The scratch directory %s does not exist!" % staging_directory, unit="AbstractFileStager")
		cls._local_scratch_dir = staging_directory
	
	def GenerateLocalFileName(self, url, readable):
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

		output_file = input_filebase + ('_%06u' % self.file_counter) + input_fileext

		if self.scratch_dir is None:
			self.scratch_dir = tempfile.mkdtemp(dir=self.get_local_scratch_dir())

			icetray.logging.log_info("Created scratch directory %s" % self.scratch_dir, unit="AbstractFileStager")

		output_path = self.scratch_dir + '/' + output_file
		self.file_counter += 1
		
		return output_path
	
	def __del__(self):
		if self.scratch_dir is not None:
			icetray.logging.log_info("Removing scratch directory %s" % self.scratch_dir, unit="AbstractFileStager")
			try:
				os.rmdir(self.scratch_dir)
			except OSError as e:
				icetray.logging.log_error("Cleanup error: %s" % str(e), unit="AbstractFileStager")

	def WillReadLater(self, url, fname):
		# not used for this implementation, this will
		# just block while copying files in StageFile()
		pass


class I3FileStagerFile(AbstractFileStager):
	"""
	Handles http://, https://, ftp://, and file:// URLs
	
	.. note:: A username/password combination may be embedded in http URLs in the
	          format specified in RFC 3986. This should only be used for "dummy"
	          shared passwords like the standard IceCube password.
	          
	          A ssl parameter is available for ssl configuration.
	"""
	def __init__(self, blocksize=2**16, ssl=None):
		AbstractFileStager.__init__(self)
		self.blocksize = blocksize
		self.ssl = ssl if ssl else {}

	def ReadSchemes(self):
		# we handle "file://" URLs
		schemes = ["http", "https", "ftp", "file"]
		return schemes
	
	def WriteSchemes(self):
		# we can't actually write anything other than local files
		return ["file"]

	def strip_auth(self, url):
		"""
		urlopen() doesn't support inline auth. Strip it out and
		construct the appropriate header by hand.
		"""
		parsed = urlparse.urlparse(url)
		auth = None
		if '@' in parsed.netloc:
			auth, netloc = parsed.netloc.split('@')
			auth = base64.encodestring(auth.encode('utf-8'))[:-1]
			parts = list(parsed)
			parts[1] = netloc
			url = urlparse.ParseResult(*parts).geturl()
		req = Request(url)
		if auth is not None:
			req.add_header('Authorization', 'Basic ' + auth.decode('utf-8'))
		return req

	def CopyFileIn(self, url, output_path):
		# parse the URL
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme

		# copy the file
		if parsed_url.scheme == "file":
			if parsed_url.netloc != "":
				icetray.logging.log_fatal("%s is not a valid URI (file:// URIs must use absolute paths)" % url, unit="I3FileStagerFile")
			input_path = parsed_url.path
			icetray.logging.log_info("Copying file %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
			shutil.copyfile(input_path, output_path)
			icetray.logging.log_info("File copied: %s to %s" % (input_path, output_path), unit="I3FileStagerFile")
		else:
			icetray.logging.log_info("Downloading %s to %s" % (url, output_path), unit="I3FileStagerFile")

			f = None
			output_file = None
			try:
				output_file = open(output_path, "wb")
				try:
					f = urlopen(self.strip_auth(url),**self.ssl)
				except TypeError:
					f = urlopen(self.strip_auth(url))
				
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
				if f is not None:
					f.close()
				if output_file is not None:
					output_file.close()
		
		return output_path

	def CopyFileOut(self, local_path, url):
		
		parsed_url = urlparse.urlparse(url, scheme="file") # use "file" as the default scheme
		if parsed_url[0] not in self.WriteSchemes():
			icetray.logging.log_fatal("Cannot handle URL scheme \"%s\": %s" % (parsed_url[0], url), unit="I3FileStagerFile")

		# copy the file
		if parsed_url[0] == "file":
			output_path = parsed_url[2]
			if parsed_url.netloc != "":
				icetray.logging.log_fatal("%s is not a valid URI (file:// URIs must use absolute paths)" % url, unit="I3FileStagerFile")
			icetray.logging.log_info("Copying file %s to %s" % (local_path, output_path), unit="I3FileStagerFile")
			shutil.copyfile(local_path, output_path)
			icetray.logging.log_info("File copied: %s to %s" % (local_path, output_path), unit="I3FileStagerFile")
		else:
			icetray.log_fatal("Can't upload to %s" % url.scheme)
		
class GridFTPStager(AbstractFileStager):
	"""
	Handles ftp:// and gsiftp:// URLs
	
	.. note:: GridFTP requires that you have a proxy certificate either in the
	          standard location or in the location specified by the environment
	          variable X509_USER_PROXY. See the `Globus Toolkit documentation <http://toolkit.globus.org/toolkit/docs/4.1/admin/docbook/gtadmin-env-var.html#id2565277>`_ for more information.
	          You will also need to `obtain a user certificate <https://wiki.icecube.wisc.edu/index.php/Using_GridFTP>`_.
	"""
	def __init__(self, globus_url_copy='globus-url-copy', options=['-nodcau', '-rst', '-cd']):
		super(type(self), self).__init__()
		self.globus_url_copy = globus_url_copy
		self.options = list(options)
	
	def ReadSchemes(self):
		return ['ftp', 'gsiftp']
	
	def WriteSchemes(self):
		return ['ftp', 'gsiftp']
	
	def CopyFileIn(self, url, local_path):
		icetray.logging.log_info("Downloading %s to %s" % (url, local_path), unit="GridFTPStager")
		proc = subprocess.Popen([self.globus_url_copy] + self.options + [url, 'file://'+os.path.abspath(local_path)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_fatal("globus-url-copy failed with status %d: %s" % (proc.returncode, stderr.strip()), unit="GridFTPStager")
		else:
			icetray.logging.log_info("Download finished: %s to %s" % (url, local_path), unit="GridFTPStager")
		
	def CopyFileOut(self, local_path, url):
		icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="GridFTPStager")
		proc = subprocess.Popen([self.globus_url_copy] + self.options + ['file://'+os.path.abspath(local_path), url], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_fatal("globus-url-copy failed with status %d: %s" % (proc.returncode, stderr.strip()), unit="GridFTPStager")
		else:
			icetray.logging.log_info("Upload finished: %s to %s" % (local_path, url), unit="GridFTPStager")

class SCPStager(AbstractFileStager):
	"""
	Handles scp:// URLs
	
	.. note:: Since there is no way to enter your password, you must have public
	          key authentication set up to use the scp stager. If you try to embed
	          the password in the URL, an error will be raised.
	"""
	def __init__(self):
		super(type(self), self).__init__()
	
	def ReadSchemes(self):
		return ['scp']
	
	def WriteSchemes(self):
		return ['scp']
	
	def CopyFileIn(self, url, local_path):
		parsed = urlparse.urlparse(url)
		remote_spec = parsed.hostname+":"+parsed.path
		if parsed.username != None:
			remote_spec = parsed.username+"@"+remote_spec
		if parsed.password != None:
			icetray.logging.log_fatal("Passwords in SCP URLs are not supported for security reasons; please use a key pair instead")
		icetray.logging.log_info("Downloading %s to %s" % (url, local_path), unit="SCPStager")
		proc = subprocess.Popen(['scp', remote_spec, os.path.abspath(local_path)], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_fatal("scp failed: "+(stderr.strip()), unit="SCPStager")
		else:
			icetray.logging.log_info("Download finished: %s to %s" % (url, local_path), unit="SCPStager")
		
	def CopyFileOut(self, local_path, url):
		parsed = urlparse.urlparse(url)
		remote_spec = parsed.hostname+":"+parsed.path
		if parsed.username != None:
			remote_spec = parsed.username+"@"+remote_spec
		if parsed.password != None:
			icetray.logging.log_fatal("Passwords in SCP URLs are not supported for security reasons; please use a key pair instead")
		icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="SCPStager")
		proc = subprocess.Popen(['scp', os.path.abspath(local_path), remote_spec], stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_fatal("scp failed: "+(stderr.strip()), unit="SCPStager")
		else:
			icetray.logging.log_info("Upload finished: %s to %s" % (local_path, url), unit="SCPStager")

class DCacheStager(AbstractFileStager):
	"""
	Handles dcap:// URLs
	"""
	def __init__(self):
		super(type(self), self).__init__()
	
	def ReadSchemes(self):
		return ['dcap']
	
	def WriteSchemes(self):
		return ['dcap']
	
	def dccp(self, *args):
		proc = subprocess.Popen(('dccp',) + args, stderr=subprocess.PIPE)
		stdout, stderr = proc.communicate()
		if proc.returncode != 0:
			icetray.logging.log_fatal("dccp failed: "+(stderr.strip()), unit="DCacheStager")
	
	def CopyFileIn(self, url, local_path):
		parsed = urlparse.urlparse(url)
		icetray.logging.log_info("Downloading %s to %s" % (url, local_path), unit="DCacheStager")
		self.dccp(parsed.path, os.path.abspath(local_path))
	
	def WillReadLater(self, url, fname):
		"""
		Tell the DCache gateway to move the file into disk cache
		"""
		parsed = urlparse.urlparse(url)
		self.dccp("-P", "-t", "3600", parsed.path)
	
	def CopyFileOut(self, local_path, url):
		parsed = urlparse.urlparse(url)
		icetray.logging.log_info("Uploading %s to %s" % (local_path, url), unit="DCacheStager")
		self.dccp(os.path.abspath(local_path), parsed.path)

