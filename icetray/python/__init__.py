from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from icecube.icetray.test_module import I3TestModuleFactory
from icecube.icetray.traysegment import traysegment, traysegment_inherit, module_altconfig
from icecube.icetray.pypick import pypick
from icecube.icetray import i3logging as logging

set_log_level = logging.set_level
set_log_level_for_unit = logging.set_level_for_unit

import atexit
def fix_logging_crash():
	# Unload any python loggers at exit to prevent Py_DECREF() after
	# interpreter destruction
	I3Logger.global_logger = None
atexit.register(fix_logging_crash)

try:
	from icecube.icetray.traydebug import I3TrayDebugger
except ImportError:
	pass
