#!/usr/bin/env python

from icecube import icetray
from I3Tray import *


try:
    t = I3Tray()

    t.AddModule("AllParametersModule", "apm",
                bool_param = True,
                int_param = 3,
                long_param = 141414141,
                double_param = 3.14159,
                string_param = "foo",
                stringvec_param = ["foo", "bar", "baz"],
                intvec_param = [1,1,2,3,4],
                omkey_param = OMKey(3,3),
                omkeyvec_param = [OMKey(1,2), OMKey(3,4)])

    t.AddModule("AllParametersModule", "apm2",
                intvec_param = [1.1,1,2,3,4])


    t.Execute(1)
    
    t.Finish()

    assert False, "that should throw with descriptive error"

except TypeError, t:

    print "OK, threw as expected"
    exit(0)
