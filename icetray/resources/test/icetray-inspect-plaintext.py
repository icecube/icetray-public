#!/usr/bin/env python3

import os
import sys

retval = os.system('icetray-inspect icetray dataclasses phys-services')

if retval != 0:
    sys.exit(1)



