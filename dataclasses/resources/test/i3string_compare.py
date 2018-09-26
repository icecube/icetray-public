#!/usr/bin/env python
from icecube import dataclasses
from icecube.icetray.I3Test import *

ENSURE( dataclasses.I3String('test') == dataclasses.I3String('test') , "I3String == I3String failed")
ENSURE( not (dataclasses.I3String('test') == dataclasses.I3String('tests')) , "I3String not == I3String failed")

ENSURE( dataclasses.I3String('test') != dataclasses.I3String('tests') , "I3String != I3String failed")
ENSURE( not (dataclasses.I3String('test') != dataclasses.I3String('test')) , "I3String not != I3String failed")

ENSURE( dataclasses.I3String('test') == 'test' , "I3String == string failed")
ENSURE( not (dataclasses.I3String('test') == 'tests') , "I3String not == string failed")

ENSURE( dataclasses.I3String('test') != 'tests' , "I3String != string failed")
ENSURE( not (dataclasses.I3String('test') != 'test') , "I3String not != string failed")
