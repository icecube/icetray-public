#!/usr/bin/env python
from icecube import icetray, dataclasses
from icecube.icetray.I3Test import *

from I3Tray import I3Units

## 
## Some examples/simple tests for FilterMasks
##

print("Testing I3FilterResults")
filter1 = dataclasses.I3FilterResult()
filter1.prescale_passed = True
filter1.condition_passed = True

filter2 = dataclasses.I3FilterResult()
filter2.prescale_passed = True
filter2.condition_passed = False

filter3 = dataclasses.I3FilterResult()
filter3.prescale_passed = False
filter3.condition_passed = False

results = dataclasses.I3FilterResultMap()

results["Filter1"] = filter1
results["Filter2"] = filter2
results["Filter3"] = filter3

print(results)

## Check if things passed Filter1

if "Filter1" in results:
    myfilter = results["Filter1"]
    # you need to check condition_passed and prescale_passed
    if myfilter.condition_passed and myfilter.prescale_passed:
        print("I passed Filter1")
    else:
        print("Error.  I should have passed Filter1")
        exit(99)

print("Testing python attr lock")
newfilter = dataclasses.I3FilterResult()

try:
    newfilter.PrescalePassed = True
    print("Aye, that should have failed partner")
    exit(99)
except:
    print("Failed as expected.  Carry on")
