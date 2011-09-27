from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from test_module import I3TestModuleFactory
from traysegment import traysegment, traysegment_inherit, module_altconfig

try:
	from traydebug import I3TrayDebugger
except ImportError:
	pass
