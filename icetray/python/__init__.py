# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube._icetray import *
__version__ = PROJECT_VERSION

from icecube.icetray.test_module import I3TestModuleFactory
from icecube.icetray.traysegment import traysegment, timedtraysegment, traysegment_inherit, module_altconfig
from icecube.icetray.pypick import pypick
from icecube.icetray import i3logging as logging
from icecube.icetray.i3tray import I3Tray

set_log_level = logging.set_level
set_log_level_for_unit = logging.set_level_for_unit

import atexit
def fix_logging_crash():
    """
    Unload any python loggers at exit to prevent Py_DECREF() after
    interpreter destruction

    :meta private:
    """
    I3Logger.global_logger = None  # type: ignore[assignment]
atexit.register(fix_logging_crash)

try:
    from icecube.icetray.traydebug import I3TrayDebugger
except ImportError:
    pass
