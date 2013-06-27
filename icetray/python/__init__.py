from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from test_module import I3TestModuleFactory
from traysegment import traysegment, traysegment_inherit, module_altconfig
from pypick import pypick
import i3logging as logging

import atexit
def fix_logging_crash():
	# Unload any python loggers at exit to prevent Py_DECREF() after
	# interpreter destruction
	I3Logger.global_logger = None
atexit.register(fix_logging_crash)

try:
	from traydebug import I3TrayDebugger
except ImportError:
	pass
