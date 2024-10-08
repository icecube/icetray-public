#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# This one *passes* though it looks like it shouldn't: You are allowed
#  to push to the generic 'outbox' (e.g. all outboxes) even if you
#  haven't added one.  What is an error is if there is a module after
#  this one.
#

from icecube.icetray import I3Tray
from icecube.icetray import I3Module

tray = I3Tray()


class NoOutboxModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Configure(self):
        pass

    def Physics(self, frame):
        self.PushFrame(frame)


tray.AddModule("ManyStreamsSource", "bottomless")
tray.AddModule("Dump", "dump")
tray.AddModule(NoOutboxModule, "no_outbox")

tray.Execute(10)
