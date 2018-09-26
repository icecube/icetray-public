#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class Mytest(unittest.TestCase):

    def test_example(self):
        t = dataclasses.TriggerKey()
        print(t)
        t2 = dataclasses.TriggerKey(dataclasses.EXTERNAL,
                dataclasses.STRING)
        print(t2)
        t3 =  dataclasses.TriggerKey(dataclasses.AMANDA_MUON_DAQ,
                dataclasses.STRING)
        print(t3)
        t4 =  dataclasses.TriggerKey(dataclasses.AMANDA_TWR_DAQ,
                dataclasses.STRING)
        print(t4)

    def test_get_source_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_source_string(dataclasses.IN_ICE),
                "IN_ICE", "Should be IN_ICE")
        self.assertEqual(tk.get_source_string(dataclasses.ICE_TOP),
                "ICE_TOP", "Should be ICE_TOP")
        self.assertEqual(tk.get_source_string(dataclasses.AMANDA_TWR_DAQ),
                "AMANDA_TWR_DAQ", "Should be AMANDA_TWR_DAQ")
        self.assertEqual(tk.get_source_string(dataclasses.EXTERNAL),
                "EXTERNAL", "Should be EXTERNAL")
        self.assertEqual(tk.get_source_string(dataclasses.GLOBAL),
                "GLOBAL", "Should be GLOBAL")
        self.assertEqual(tk.get_source_string(dataclasses.AMANDA_MUON_DAQ),
                "AMANDA_MUON_DAQ", "Should be AMANDA_MUON_DAQ")
        self.assertEqual(tk.get_source_string(dataclasses.SPASE),
                "SPASE", "Should be SPASE")
        self.assertEqual(tk.get_source_string(dataclasses.UNKNOWN_SOURCE),
                "UNKNOWN_SOURCE", "Should be UNKNOWN_SOURCE")

    def test_get_source_from_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_source_from_string("IN_ICE"),
                dataclasses.IN_ICE, "Should be IN_ICE")
        self.assertEqual(tk.get_source_from_string("ICE_TOP"),
                dataclasses.ICE_TOP, "Should be ICE_TOP")
        self.assertEqual(tk.get_source_from_string("AMANDA_TWR_DAQ"),
                dataclasses.AMANDA_TWR_DAQ, "Should be AMANDA_TWR_DAQ")
        self.assertEqual(tk.get_source_from_string("EXTERNAL"),
                dataclasses.EXTERNAL, "Should be EXTERNAL")
        self.assertEqual(tk.get_source_from_string("GLOBAL"),
                dataclasses.GLOBAL, "Should be GLOBAL")
        self.assertEqual(tk.get_source_from_string("AMANDA_MUON_DAQ"),
                dataclasses.AMANDA_MUON_DAQ, "Should be AMANDA_MUON_DAQ")
        self.assertEqual(tk.get_source_from_string("SPASE"),
                dataclasses.SPASE, "Should be SPASE")
        self.assertEqual(tk.get_source_from_string("UNKNOWN_SOURCE"),
                dataclasses.UNKNOWN_SOURCE, "Should be UNKNOWN_SOURCE")

    def test_get_type_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_type_string(dataclasses.SIMPLE_MULTIPLICITY),
                "SIMPLE_MULTIPLICITY", "Should be SIMPLE_MULTIPLICITY")
        self.assertEqual(tk.get_type_string(dataclasses.CALIBRATION),
                "CALIBRATION", "Should be CALIBRATION")
        self.assertEqual(tk.get_type_string(dataclasses.MIN_BIAS),
                "MIN_BIAS", "Should be MIN_BIAS")
        self.assertEqual(tk.get_type_string(dataclasses.THROUGHPUT),
                "THROUGHPUT", "Should be THROUGHPUT")
        self.assertEqual(tk.get_type_string(dataclasses.TWO_COINCIDENCE),
                "TWO_COINCIDENCE", "Should be TWO_COINCIDENCE")
        self.assertEqual(tk.get_type_string(dataclasses.THREE_COINCIDENCE),
                "THREE_COINCIDENCE", "Should be THREE_COINCIDENCE")
        self.assertEqual(tk.get_type_string(dataclasses.MERGED),
                "MERGED", "Should be MERGED")
        self.assertEqual(tk.get_type_string(dataclasses.SLOW_PARTICLE),
                "SLOW_PARTICLE", "Should be SLOW_PARTICLE")
        self.assertEqual(tk.get_type_string(dataclasses.FRAGMENT_MULTIPLICITY),
                "FRAGMENT_MULTIPLICITY", "Should be FRAGMENT_MULTIPLICITY")
        self.assertEqual(tk.get_type_string(dataclasses.STRING),
                "STRING", "Should be STRING")
        self.assertEqual(tk.get_type_string(dataclasses.VOLUME),
                "VOLUME", "Should be VOLUME")
        self.assertEqual(tk.get_type_string(dataclasses.SPHERE),
                "SPHERE", "Should be SPHERE")
        self.assertEqual(tk.get_type_string(dataclasses.UNBIASED),
                "UNBIASED", "Should be UNBIASED")
        self.assertEqual(tk.get_type_string(dataclasses.SPASE_2),
                "SPASE_2", "Should be SPASE_2")
        self.assertEqual(tk.get_type_string(dataclasses.UNKNOWN_TYPE),
                "UNKNOWN_TYPE", "Should be UNKNOWN_TYPE")

    def test_get_type_from_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_type_from_string("SIMPLE_MULTIPLICITY"),
                dataclasses.SIMPLE_MULTIPLICITY,
                "Should be SIMPLE_MULTIPLICITY")
        self.assertEqual(tk.get_type_from_string("CALIBRATION"),
                dataclasses.CALIBRATION,
                "Should be CALIBRATION")
        self.assertEqual(tk.get_type_from_string("MIN_BIAS"),
                dataclasses.MIN_BIAS,
                "Should be MIN_BIAS")
        self.assertEqual(tk.get_type_from_string("THROUGHPUT"),
                dataclasses.THROUGHPUT,
                "Should be THROUGHPUT")
        self.assertEqual(tk.get_type_from_string("TWO_COINCIDENCE"),
                dataclasses.TWO_COINCIDENCE,
                "Should be TWO_COINCIDENCE")
        self.assertEqual(tk.get_type_from_string("THREE_COINCIDENCE"),
                dataclasses.THREE_COINCIDENCE,
                "Should be THREE_COINCIDENCE")
        self.assertEqual(tk.get_type_from_string("MERGED"),
                dataclasses.MERGED,
                "Should be MERGED")
        self.assertEqual(tk.get_type_from_string("SLOW_PARTICLE"),
                dataclasses.SLOW_PARTICLE,
                "Should be SLOW_PARTICLE")
        self.assertEqual(tk.get_type_from_string("FRAGMENT_MULTIPLICITY"),
                dataclasses.FRAGMENT_MULTIPLICITY,
                "Should be FRAGMENT_MULTIPLICITY")
        self.assertEqual(tk.get_type_from_string("STRING"),
                dataclasses.STRING,
                "Should be STRING")
        self.assertEqual(tk.get_type_from_string("VOLUME"),
                dataclasses.VOLUME,
                "Should be VOLUME")
        self.assertEqual(tk.get_type_from_string("SPHERE"),
                dataclasses.SPHERE,
                "Should be SPHERE")
        self.assertEqual(tk.get_type_from_string("UNBIASED"),
                dataclasses.UNBIASED,
                "Should be UNBIASED")
        self.assertEqual(tk.get_type_from_string("SPASE_2"),
                dataclasses.SPASE_2,
                "Should be SPASE_2")
        self.assertEqual(tk.get_type_from_string("UNKNOWN_TYPE"),
                dataclasses.UNKNOWN_TYPE,
                "Should be UNKNOWN_TYPE")



unittest.main()
