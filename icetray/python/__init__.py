# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube._icetray import (
    GIT_BRANCH, GIT_REVISION, GIT_URL, I3Bool, I3ConditionalModule, I3Configuration, I3Context, I3Default, I3Frame,
    I3FrameMixer, I3FrameObject, I3Int, I3LogLevel, I3Logger, I3Module, I3NullLogger, I3PacketModule, I3PhysicsUsage,
    I3PrintfLogger, I3RUsage, I3SyslogLogger, I3TrayInfo, I3TrayInfoService, I3Units, I3_PLATFORM, MyService, OMKey,
    PROJECT_VERSION, _I3TrayBase, get_log_level_for_unit, get_scratch_directory, is_module, is_service, load,
    map_OMKey_int, map_int_int, map_string_I3Configuration, map_string_I3PhysicsUsage, map_string_pyobject,
    module_default_config, modules, param_setter, projects, services, set_scratch_directory, vector_I3Frame_Stream,
    vector_OMKey, vector_char, vector_double, vector_int, vector_short, vector_string, vector_ulong, vector_unsigned,
    vector_vector_int, vector_vector_string
)
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
