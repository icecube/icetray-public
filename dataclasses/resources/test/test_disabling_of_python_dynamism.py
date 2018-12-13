#!/usr/bin/env python
from icecube.icetray.I3Test import *
from icecube import dataclasses as dc
import inspect
from icecube.icetray import test_disabling_of_python_dynamism as test_freeze

# test_all_classes() fails for classes that don't have a nullary constructor.
skip_these = ["I3SuperDST","I3SuperDSTTrigger","I3SuperDSTTriggerSeries","I3RecoPulseSeriesMapMask",\
              "I3RecoPulseSeriesMapUnion","DroopedSPETemplate",\
              "map_indexing_suite_map_Subdetector_I3TriggerReadoutConfig_entry",
              "PairDoubleDouble", "I3Matrix", "I3RecoPulseSeriesMapApplySPECorrection",
              "I3RecoPulseSeriesMapCombineByModule"]
ENSURE( test_freeze.test_all_classes(dc, skip_these) , "not cool" )

print("PASS")
