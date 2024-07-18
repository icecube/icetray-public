#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Test i3logger
"""

from unittest import TestCase, main

from icecube import icetray

class LoggerTest(TestCase):

    def setUp(self):
        self.log = icetray.I3Logger()

    def test_default_level(self):
        lvl = icetray.get_log_level_for_unit('blah')
        self.assertIn(lvl, [icetray.I3LogLevel.LOG_INFO, icetray.I3LogLevel.LOG_NOTICE])
    
    def test_set_level_for_unit(self):
        for target in [icetray.I3LogLevel.LOG_WARN, icetray.I3LogLevel.LOG_ERROR]:
            self.log.set_level_for_unit('blah2', target)
            self.assertEqual(self.log.get_level_for_unit('blah2'), target)

    def test_global_logger_type(self):
        self.assertIsInstance(self.log.global_logger, icetray.I3Logger)

    def test_logger_base(self):
        with self.assertRaises(NotImplementedError):
            self.log.log(icetray.I3LogLevel.LOG_INFO, 'blah', 'file', 123, 'func', 'message')
        self.log.global_logger.log(icetray.I3LogLevel.LOG_INFO, 'blah', 'file', 123, 'func', 'message')

class BadModuleError(RuntimeError):
    pass

class BadModule(icetray.I3Module):
    def Process(self):
        raise BadModuleError

class LogModule(icetray.I3Module):
    def Process(self):
        icetray.logging.log_error("ruh roh")

class ThrowingBridgeError(RuntimeError):
    pass

class ThrowingBridge(icetray.logging.LoggingBridge):
    def log(self, level, unit, file, line, func, msg):
        if level == icetray.I3LogLevel.LOG_ERROR:
            raise ThrowingBridgeError(msg)
        return super().log(level, unit, file, line, func, msg)

class ConsoleLoggingSetup(TestCase):
    def setUp(self):
        self._global_logger = icetray.I3Logger.global_logger
        icetray.logging.console()

    def tearDown(self) -> None:
        icetray.I3Logger.global_logger = self._global_logger

class ConsoleLoggingTest(ConsoleLoggingSetup):
    """
    See: https://github.com/icecube/icetray/issues/3465
    """
    def test(self):
        tray = icetray.I3Tray()
        tray.AddModule(BadModule)
        with self.assertRaises(BadModuleError) as exc_context:
            tray.Execute(1)
        self.assertIsNone(exc_context.exception.__context__)

class ConsoleLoggingChainedErrorTest(ConsoleLoggingSetup):
    """
    Errors raised in logging are chained to current exception
    """
    def setUp(self):
        super().setUp()
        icetray.I3Logger.global_logger = ThrowingBridge()

    def test_inner_error(self):
        tray = icetray.I3Tray()
        tray.AddModule(BadModule)
        with self.assertRaises(ThrowingBridgeError) as exc_context:
            tray.Execute(1)
        self.assertIsInstance(exc_context.exception.__context__, BadModuleError)
        self.assertIsNone(exc_context.exception.__context__.__context__)

    def test_outer_error(self):
        tray = icetray.I3Tray()
        tray.AddModule(LogModule)
        with self.assertRaises(ThrowingBridgeError) as exc_context:
            tray.Execute(1)
        self.assertRegex(exc_context.exception.args[0], "Exception thrown")
        self.assertIsInstance(exc_context.exception.__context__, ThrowingBridgeError)
        self.assertEqual(exc_context.exception.__context__.args[0], "ruh roh")
        self.assertIsNone(exc_context.exception.__context__.__context__)


if __name__ == '__main__':
    main()
