from icecube import icetray
from icecube._phys_services import *

del icetray

# tableio exposes classes in _phys_services.converters
try:
    import icecube.tableio
    from icecube._phys_services import converters
except ImportError:
    ...
