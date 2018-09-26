# create the registry first, so that compiled bookers can be added
from icecube.tableio.registry import I3ConverterRegistry

from icecube.load_pybindings import load_pybindings
import icecube.icetray # be nice and pull in our dependencies
import icecube.dataclasses
import sys
load_pybindings(__name__,__path__)

from icecube.tableio.I3TableWriterModule import I3TableWriter, default

from icecube.tableio import types

if sys.version_info[0] >= 3:
	from icecube.tableio.enum3 import enum as enum
else:
	from icecube.tableio.enum2 import enum as enum

# clean up the local dictionary
del icecube

