# create the registry first, so that compiled bookers can be added
from icecube.tableio.registry import I3ConverterRegistry

import icecube.icetray # be nice and pull in our dependencies
import icecube.dataclasses
import sys
from icecube._tableio import *

from icecube.tableio.I3TableWriterModule import I3TableWriter, default

from icecube.tableio import types

from icecube.tableio.enum3 import enum as enum

# clean up the local dictionary
del icecube

