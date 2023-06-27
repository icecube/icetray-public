#!/usr/bin/env python3

import unittest
from icecube import dataclasses
from icecube.dataclasses import I3TriggerHierarchy,I3Trigger,TriggerKey,TriggerKeyVector

#IceTop and InIce Simple Majority Trigger keys
ITSMT = dataclasses.TriggerKey(dataclasses.ICE_TOP,dataclasses.SIMPLE_MULTIPLICITY,102)
IT7HG = dataclasses.TriggerKey(dataclasses.ICE_TOP,dataclasses.SIMPLE_MULTIPLICITY,30043)
IISMT8 = dataclasses.TriggerKey(dataclasses.IN_ICE,dataclasses.SIMPLE_MULTIPLICITY,1006)
IISMT3 = dataclasses.TriggerKey(dataclasses.IN_ICE,dataclasses.SIMPLE_MULTIPLICITY,1011)

def getTrigger(key,fired,length,time):
  """
  returns a trigger with given key,length,time.
  """
  trig = I3Trigger()
  trig.key = key
  trig.fired = fired
  trig.length = length
  trig.time = time
  return trig

ITTrig = getTrigger(ITSMT,True,1,10)
IITrig8 = getTrigger(IISMT8,True,2,10)
IITrig3 = getTrigger(IISMT3,True,3,10)

def getTriggerHierarchy(triggerList):
  '''
  returns I3TriggerHierarchy with triggers in triggerList
  '''
  testTrigH = I3TriggerHierarchy()
  for itrigger in triggerList:
    testTrigH.insert(itrigger)
  return testTrigH

testTrigH_6 = getTriggerHierarchy([ITTrig,ITTrig,IITrig8,IITrig8,IITrig3,IITrig3,IITrig3,IITrig3])
testTrigH_2 = getTriggerHierarchy([ITTrig,ITTrig,ITTrig,ITTrig,IITrig8,IITrig3])

class TestCountEqualKeys(unittest.TestCase):
    def test_inice_trigger_count(self):
      testTrigH_6 = getTriggerHierarchy([ITTrig,ITTrig,IITrig8,IITrig8,IITrig3,IITrig3,IITrig3,IITrig3])
      testTrigH_2 = getTriggerHierarchy([ITTrig,ITTrig,ITTrig,ITTrig,IITrig8,IITrig3])
      inice_trigger_keys = TriggerKeyVector()
      inice_trigger_keys.append(IISMT8)
      inice_trigger_keys.append(IISMT3)
      self.assertEqual(testTrigH_2.count(inice_trigger_keys), 2,"InIce trigger count should be 2")
      self.assertEqual(testTrigH_6.count(inice_trigger_keys), 6,"InIce trigger count should be 6")

if __name__ == '__main__':
    unittest.main()

