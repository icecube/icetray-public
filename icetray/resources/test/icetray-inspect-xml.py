#!/usr/bin/env python

import os
import sys

retval = os.system('icetray-inspect -x -p icetray dataclasses phys-services')

if retval != 0:
    sys.exit(1)



