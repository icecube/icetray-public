#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from optparse import OptionParser
parser = OptionParser()
parser.add_option('-i','--infile',
                  dest='INFILE',
                  help='I3File to read.')
(options,args) = parser.parse_args()

from icecube.icetray import I3Tray
from icecube import icetray, dataio, phys_services
import sys

tray = I3Tray()
tray.AddModule('I3Reader', Filename = options.INFILE)
tray.AddModule('I3BadDOMAuditor', Pulses=['OfflinePulses', 'OfflineIceTopHLCTankPulses'])
tray.Execute()

