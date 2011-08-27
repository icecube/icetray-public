#!/usr/bin/env python

from icecube import icetray, phys_services, examples
from I3Tray import *

# Ensure that services can be passed directly (as a wrapped pointer),
# or via the context (as a string).

tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory", "rng")
rng = phys_services.I3GSLRandomService(1337)

# Pass a string to a module that expects to get a pointer.
# I3Configuration::Get() should fall back to getting the
# pointer from the context.
tray.AddModule("UseRandom", "r1",
	I3RandomService="rng",
)

# Regular parameter conversions should still work afterwards.
# If not, someone forgot to clear an error flag set by 
# boost::python::extract.
tray.AddModule("AllParametersModule", "ope",
	int_param = 3,
)

tray.AddModule("TrashCan", "YesWeCan")
tray.Execute(1)
tray.Finish()
