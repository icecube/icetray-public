#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3Trigger(unittest.TestCase):

    def test_I3Trigger_equality(self):
        key1 = dataclasses.TriggerKey()
        key1.source    = key1.source.IN_ICE
        key1.type      = key1.type.MIN_BIAS
        key1.subtype   = key1.subtype.M18
        key1.config_id = 101
        trig1 = dataclasses.I3Trigger()
        trig1.time   = 350 * I3Units.ns
        trig1.length = 6.55 * I3Units.microsecond
        trig1.fired  = False
        trig1.key    = key1

        key2 = dataclasses.TriggerKey()
        key2.source    = key2.source.IN_ICE
        key2.type      = key2.type.MIN_BIAS
        key2.subtype   = key2.subtype.M18
        key2.config_id = 101
        trig2 = dataclasses.I3Trigger()
        trig2.time   = 350 * I3Units.ns
        trig2.length = 6.55 * I3Units.microsecond
        trig2.fired  = False
        trig2.key    = key2

        self.assertEqual(trig1, trig2, "these should be the same.")

        trig3 = dataclasses.I3Trigger()
        self.assertNotEqual(trig1, trig3, "these should not be the same.")

unittest.main()
