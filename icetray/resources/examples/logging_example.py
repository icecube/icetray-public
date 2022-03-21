#!/usr/bin/env python3

'''
This script doesn't really do much.  For output, you should
see 10 frames with output like so:

------------------------- This is frame number 7 -------------------------
[ I3Frame  (Physics):
]
INFO (LoggingExampleModule): The human head weighs 8lbs. (LoggingExampleModule.cxx:40 in virtual void LoggingExampleModule::Process())
WARN (LoggingExampleModule): Danger, Will Robinson! (LoggingExampleModule.cxx:41 in virtual void LoggingExampleModule::Process())
ERROR (LoggingExampleModule): PC LOAD LETTER (LoggingExampleModule.cxx:42 in virtual void LoggingExampleModule::Process())
FATAL (LoggingExampleModule): halt and catch fire (LoggingExampleModule.cxx:44 in virtual void LoggingExampleModule::Process())
(cout) fatal: halt and catch fire (in virtual void LoggingExampleModule::Process())

'''

from icecube import icetray
from I3Tray import I3Tray

icetray.set_log_level_for_unit('LoggingExampleModule', icetray.I3LogLevel.LOG_INFO)

tray = I3Tray()
tray.AddModule("BottomlessSource")
tray.AddModule("LoggingExampleModule")
tray.AddModule("Dump")
tray.Execute(10)

