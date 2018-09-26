/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author tschmidt
*/
#include <I3Test.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3TriggerHierarchy);

TEST(I3TriggerHierarchyUtils)
{
  I3TriggerHierarchy t;
  I3TriggerHierarchy::iterator iter;
  I3TriggerHierarchy::iterator jter;
  I3TriggerHierarchy::iterator kter;
  
  iter = t.insert(t.begin(), I3Trigger());
  iter->GetTriggerKey() = TriggerKey(TriggerKey::GLOBAL, TriggerKey::MERGED);
  
  jter = t.append_child(iter, I3Trigger());
  jter->GetTriggerKey() = TriggerKey(TriggerKey::GLOBAL,
                                     TriggerKey::TWO_COINCIDENCE, 1);
  kter = t.append_child(jter, I3Trigger());
  kter->GetTriggerKey() = TriggerKey(TriggerKey::IN_ICE,
                                     TriggerKey::SIMPLE_MULTIPLICITY, 2);
  kter = t.append_child(jter, I3Trigger());
  kter->GetTriggerKey() = TriggerKey(TriggerKey::AMANDA_MUON_DAQ,
                                     TriggerKey::UNKNOWN_TYPE, 3);
  
  jter = t.append_child(iter, I3Trigger());
  jter->GetTriggerKey() = TriggerKey(TriggerKey::GLOBAL,
                                     TriggerKey::THROUGHPUT, 4);
  kter= t.append_child(jter, I3Trigger());
  kter->GetTriggerKey() = TriggerKey(TriggerKey::ICE_TOP,
                                     TriggerKey::SIMPLE_MULTIPLICITY, 5);


  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey::EXTERNAL));
  ENSURE_EQUAL(1u, I3TriggerHierarchyUtils::Count(t, TriggerKey::AMANDA_MUON_DAQ));
  ENSURE_EQUAL(3u, I3TriggerHierarchyUtils::Count(t, TriggerKey::GLOBAL));
  
  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey::MIN_BIAS));
  ENSURE_EQUAL(2u, I3TriggerHierarchyUtils::Count(t, TriggerKey::SIMPLE_MULTIPLICITY));
  
  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey::IN_ICE,
                                                  TriggerKey::CALIBRATION));
  ENSURE_EQUAL(1u, I3TriggerHierarchyUtils::Count(t, TriggerKey::GLOBAL,
                                                  TriggerKey::MERGED));
  
  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey(TriggerKey::GLOBAL,
                                                                TriggerKey::MERGED, 1)));
  ENSURE_EQUAL(1u, I3TriggerHierarchyUtils::Count(t, TriggerKey(TriggerKey::GLOBAL,
                                                                TriggerKey::MERGED)));
  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey(TriggerKey::GLOBAL,
                                                                TriggerKey::THROUGHPUT)));
  ENSURE_EQUAL(0u, I3TriggerHierarchyUtils::Count(t, TriggerKey(TriggerKey::ICE_TOP,
                                                                TriggerKey::SIMPLE_MULTIPLICITY,
                                                                6)));
  ENSURE_EQUAL(1u, I3TriggerHierarchyUtils::Count(t, TriggerKey(TriggerKey::ICE_TOP,
                                                                TriggerKey::SIMPLE_MULTIPLICITY,
                                                                5)));
                                                               
  ENSURE(I3TriggerHierarchyUtils::Find(t, TriggerKey::EXTERNAL) == t.end());
  iter = I3TriggerHierarchyUtils::Find(t, TriggerKey::AMANDA_MUON_DAQ);
  ENSURE(iter != t.end());
  ENSURE(iter->GetTriggerKey().GetType() == TriggerKey::UNKNOWN_TYPE);
  
  iter = I3TriggerHierarchyUtils::Find(t, TriggerKey::SIMPLE_MULTIPLICITY);
  ENSURE(iter != t.end());
  ENSURE(iter->GetTriggerKey().GetSource() == TriggerKey::IN_ICE);
  ENSURE(iter->GetTriggerKey().GetConfigID() == 2);
  jter = I3TriggerHierarchyUtils::Find(t, TriggerKey::SIMPLE_MULTIPLICITY, ++iter);
  ENSURE(jter != t.end());
  ENSURE(jter->GetTriggerKey().GetSource() == TriggerKey::ICE_TOP);
  ENSURE(jter->GetTriggerKey().GetConfigID() == 5);
  ENSURE(++jter == t.end());

  unsigned int counter = 0u;
  iter = t.begin();
  while(iter != t.end())
  {
    iter = I3TriggerHierarchyUtils::Find(t, TriggerKey::SIMPLE_MULTIPLICITY, iter);
    if(iter == t.end()) break;
    ++counter;
    ++iter;
  }
  ENSURE_EQUAL(2u, counter);
}
