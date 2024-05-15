# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray
from icecube._phys_services import *
from icecube._phys_services import I3Calculator

del icetray

# tableio exposes classes in _phys_services.converters
try:
    import icecube.tableio
    from icecube._phys_services import converters
except ImportError:
    ...
