/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author Troy D. Straszheim

    @todo

*/
#include <I3Test.h>
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/TriggerKey.h"
// this is a typical minimal testsuite

// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3TriggerTest);

/**
 * Checks assignment/copy construction
 */
// the string here is used to identify this specific test.
TEST(assignment_copy)
{
  I3Trigger h, j;

  //Check that constructor preset values are set properly
  ENSURE(h.GetTriggerFired() == false);
  ENSURE(h.GetTriggerTime() == 0.0);
  ENSURE(h.GetTriggerLength() == 0.0);
  //Now set some values.
  j.SetTriggerTime(12345.67);
  j.SetTriggerLength(1567.89);
  j.SetTriggerFired(true);
  j.GetTriggerKey().SetSource(TriggerKey::AMANDA_MUON_DAQ);
  //test assignment one object to another
  h = j;
  //Check it correct.
  ENSURE_DISTANCE(0.1, 0.1, 0.0001,"ensure test");
  ENSURE_DISTANCE(j.GetTriggerTime(), h.GetTriggerTime(), 0.001,
		  "trigger time simple assignment");
  ENSURE_DISTANCE(j.GetTriggerLength(), h.GetTriggerLength(), 0.001,
		  "trigger length simple assignment");
  ENSURE(j.GetTriggerFired() == j.GetTriggerFired());

  ENSURE(j.GetTriggerKey().GetSource() == TriggerKey::AMANDA_MUON_DAQ);
  ENSURE(TriggerKey::GetSourceFromString("AMANDA_MUON_DAQ") == TriggerKey::AMANDA_MUON_DAQ);
}

