from icecube import icetray
from icecube.icetray import i3inspect
import sys

# This is horrible hack to get around the except foo, e -> except foo as e
# transition. Once we drop Python < 2.6 support, the "3" version should be
# reintegrated.

def i3inspect_getconfig(mod):
	try:
		return i3inspect.get_configuration(mod)
	except RuntimeError as e:
		sys.stderr.write("Error constructing '%s': %s" % (mod, e))
		raise

def i3inspect_load(project):
	try:
		icetray.load(project, False)
		return True
	except RuntimeError as e:
		sys.stderr.write("Ignoring '%s': %s" % (project, e))
		return False

