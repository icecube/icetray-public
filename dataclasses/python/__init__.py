# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import icecube.icetray # be nice and pull in our dependencies

# XXX Work around a stupid bug with certain combinations of Boost, compiler, and
# RTLD where pulling in libdataclasses as a dependency of the pybindings will
# cause corruption of the internal state of the extended_type_info database
# when classes in dataclasses are inherited from in certain circumstances I
# can't identify.
# Observed on: FreeBSD 10 + Boost 1.52 + clang 3.2 when loading mutineer
# (of all things)
icecube.icetray.load('dataclasses', False)
from icecube._dataclasses import *
from icecube._dataclasses import fft

# Add some aliases for naming consistency
I3RecoPulseSeries = vector_I3RecoPulse
I3RecoHitSeries = vector_I3RecoHit
I3MCHitSeries = vector_I3MCHit

# Aliases for collections factored out into their own implementation files
I3TriggerStatusMap = Map_TriggerKey_I3TriggerStatus
I3VEMCalibrationMap = Map_OMKey_I3VEMCalibration
I3IceTopSLCCalibrationMap = Map_OMKey_I3IceTopSLCCalibration
I3DOMCalibrationMap = Map_OMKey_I3DOMCalibration
I3DOMStatusMap = Map_OMKey_I3DOMStatus
ParticleTypeVect = ListParticleType
setattr(TankKey, "I3VectorTankKey", I3VectorTankKey)  # noqa: B010

del icecube

# tableio exposes classes in _dataclasses.converters
try:
    import icecube.tableio
    from icecube._dataclasses import converters
except ImportError:
    ...
