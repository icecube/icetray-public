#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Ensure that the most specific converter is always chosen for each type, but that
types can be written by converters registered for base classes
"""

# This is uglier than the corresponding C++ test, but querying which converter
# is selected would require vastly more plumbing, so instead we just do things
# the sloppy way with temporary files and hoping that hdfwriter is around.

import os
import unittest
from contextlib import contextmanager

from icecube import phys_services  # noqa: F401
from icecube import dataclasses, icetray, tableio
from icecube.icetray import I3Tray

icetray.logging.set_level_for_unit("I3Tray", "WARN")

class Foo(icetray.I3FrameObject):
    def __init__(self):
        icetray.I3FrameObject.__init__(self)
        self.a = 5

class Foo2(Foo):
    def __init__(self):
        Foo.__init__(self)
        self.a *= 2

class Bar(Foo):
    def __init__(self):
        Foo.__init__(self)
        self.b = 7

class FooConverter(tableio.I3Converter):
    booked = Foo
    def CreateDescription(self,obj):
        desc = tableio.I3TableRowDescription()
        desc.add_field("a",tableio.types.Float64,"","Parameter A")
        return desc
    def FillRows(self,obj,rows):
        rows["a"]=obj.a
        return 1

tableio.I3ConverterRegistry.register(FooConverter)

class BarConverter(tableio.I3Converter):
    booked = Bar
    def CreateDescription(self,obj):
        desc = tableio.I3TableRowDescription()
        desc.add_field("a",tableio.types.Float64,"","Parameter A")
        desc.add_field("b",tableio.types.Float64,"","Parameter B")
        return desc
    def FillRows(self,obj,rows):
        rows["a"]=obj.a
        rows["b"]=obj.b
        return 1

class ExtendedBarConverter(tableio.I3Converter):
    def CreateDescription(self,obj):
        desc = tableio.I3TableRowDescription()
        desc.add_field("a2",tableio.types.Float64,"","Parameter A times 2")
        return desc
    def FillRows(self,obj,rows):
        rows["a2"]=obj.a*2
        return 1

tableio.I3ConverterRegistry.register(BarConverter)

def add_eventheader(frame):
    eh = dataclasses.I3EventHeader()
    eh.run_id = 1
    eh.event_id = add_eventheader.event_id
    add_eventheader.event_id += 1
    frame['I3EventHeader'] = eh
add_eventheader.event_id = 1

def make_make_object(type):
    def make_object(frame):
        obj = type()
        frame["Object"] = obj
    return make_object

def try_to_write(type,file,keys=["Object"],types=[]):
    try:
        from icecube.hdfwriter import I3HDFWriter
    except ImportError:
        raise unittest.SkipTest("hdfwriter project missing")
    tray = I3Tray()
    tray.AddModule('I3InfiniteSource')
    tray.AddModule(add_eventheader, Streams=[icetray.I3Frame.DAQ])
    tray.AddModule("I3NullSplitter", "nullsplit", SubEventStreamName="nullsplit")
    tray.AddModule(make_make_object(type))
    tray.AddSegment(I3HDFWriter,
        Output=file,
        Keys=keys,
        Types=types,
        SubEventStreams=["nullsplit"],
    )
    tray.Execute(1)

class BaseConverter(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.test_output = "test_" + self.__class__.__name__ + ".hdf5"
    def setUp(self):
        try_to_write(Foo, self.test_output)
    def tearDown(self):
        os.unlink(self.test_output)
    def testGeneric(self):
        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")
        with tables.open_file(self.test_output) as hdf:
            table=hdf.get_node("/Object")
            self.assertIsNotNone(table, "Object table exists")
            self.assertIn("a", table.colnames, "'A' parameter was recorded")
            self.assertTrue("b" not in table.colnames, "'B' parameter does not exist")

class GenericSubclassConverter(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.test_output = "test_" + self.__class__.__name__ + ".hdf5"
    def setUp(self):
        try_to_write(Foo2,self.test_output)
    def tearDown(self):
        os.unlink(self.test_output)
    def testGeneric(self):
        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")
        with tables.open_file(self.test_output) as hdf:
            table = hdf.get_node("/Object")
            self.assertIsNotNone(table, "Object table exists")
            self.assertIn("a", table.colnames, "'A' parameter was recorded")
            self.assertTrue("b" not in table.colnames, "'B' parameter does not exist")

class SpecificSubclassConverter(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.test_output = "test_" + self.__class__.__name__ + ".hdf5"
    def setUp(self):
        print("class name: " + self.__class__.__name__)
        try_to_write(Bar,self.test_output)
    def tearDown(self):
        os.unlink(self.test_output)
    def testGeneric(self):
        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")
        with tables.open_file(self.test_output) as hdf:
            table = hdf.get_node("/Object")
            self.assertIsNotNone(table, "Object table exists")
            self.assertIn("a", table.colnames, "'A' parameter was recorded")
            self.assertIn("b", table.colnames, "'B' parameter was recorded")

class ComposedConverter(unittest.TestCase):

    @contextmanager
    def table(self, klass=Bar, fname="test.hdf5", keys=["Object"], types=[]):
        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")
        try:
            try_to_write(klass, fname, keys, types)
            with tables.open_file(fname) as hdf:
                table = hdf.get_node("/Object")
                self.assertIsNotNone(table, "Object table exists")
                yield table
        finally:
            os.unlink(fname)

    @contextmanager
    def _check_table(self, **kwargs):
        with self.table(**kwargs) as table:
            self.assertIn("a", table.colnames, "'A' parameter was recorded")
            self.assertIn("b", table.colnames, "'B' parameter was recorded")
            yield table

    def testSingleConverterByKey(self):
        self._check_table(keys=[{"key": "Object", "converter": BarConverter()}])

    def testSingleComposedConverterByKey(self):
        self._check_table(keys=[{"key": "Object", "converter": [BarConverter()]}])

    def testComposedConverterByKey(self):
        with self._check_table(keys=[{"key": "Object", "converter": [BarConverter(), ExtendedBarConverter()]}]) as table:
            self.assertIn("a2", table.colnames)
            values = table.read()
            self.assertEqual(values['a']*2, values['a2'])

    def testSingleConverterByType(self):
        self._check_table(keys=[], types=[{"type": Bar, "converter": BarConverter()}])

    def testSingleComposedConverterByType(self):
        self._check_table(keys=[], types=[{"type": Bar, "converter": [BarConverter()]}])

class CanConvert(unittest.TestCase):
    def testConversion(self):
        f=Foo()
        f2=Foo2()
        b=Bar()

        fc=FooConverter()
        bc=BarConverter()

        self.assertEqual(fc.CanConvert(f),tableio.ConvertState.ExactConversion)
        self.assertEqual(fc.CanConvert(f2),tableio.ConvertState.InexactConversion)
        self.assertEqual(fc.CanConvert(b),tableio.ConvertState.InexactConversion)

        self.assertEqual(bc.CanConvert(f),tableio.ConvertState.NoConversion)
        self.assertEqual(bc.CanConvert(f2),tableio.ConvertState.NoConversion)
        self.assertEqual(bc.CanConvert(b),tableio.ConvertState.ExactConversion)

if __name__ == "__main__":
    unittest.main(verbosity=2)
