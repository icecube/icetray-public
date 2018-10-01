#!/usr/bin/env python

from icecube import icetray, phys_services
from I3Tray import *

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

tray.AddService("I3GSLRandomServiceFactory", "rng")
rng = phys_services.I3GSLRandomService(1337)

# Pass a string to a module that expects to get a pointer.
# I3Configuration::Get() should fall back to getting the
# pointer from the context.
tray.AddModule(UseRandom, I3RandomService = "rng")

# Regular parameter conversions should still work afterwards.
# If not, someone forgot to clear an error flag set by 
# boost::python::extract.
tray.AddModule("AllParametersModule", int_param = 3)

tray.Execute(1)

