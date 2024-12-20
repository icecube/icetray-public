#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray, phys_services
from icecube.icetray import I3Tray

# Ensure that services can be passed directly (as a wrapped pointer),
# or via the context (as a string).

class UseRandom(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("I3RandomService", "My random service", None)

    def Configure(self):
        self.rs = self.GetParameter("I3RandomService")
        assert(self.rs)

tray = I3Tray()

tray.AddService("I3MTRandomServiceFactory", "rng")
rng = phys_services.I3MTRandomService(1337)

# Pass a string to a module that expects to get a pointer.
# I3Configuration::Get() should fall back to getting the
# pointer from the context.
tray.AddModule(UseRandom, I3RandomService = "rng")

# Regular parameter conversions should still work afterwards.
# If not, someone forgot to clear an error flag set by
# boost::python::extract.
tray.AddModule("AllParametersModule", int_param = 3)

tray.Execute(1)

