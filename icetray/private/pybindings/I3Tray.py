__all__ = ["I3Tray", "I3Units", "OMKey", "NaN", "Inf"]

import warnings
warnings.warn("Using `import I3Tray` or `from I3Tray import *` is now considered depricated. "
              "Please switch to using `from icecube.icetray import I3Tray`", stacklevel=2)

NaN = float('NaN')
Inf = float('inf')
from icecube.icetray import I3Tray, I3Units, OMKey
