/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Context.h>
#include <icetray/OMKey.h>
#include <boost/lexical_cast.hpp>
#include <string>
using namespace std;

TEST_GROUP(LexicalCasts);

TEST(OMKey_roundtrip)
{
  // OMKey goes round-trip
  OMKey k(3,14);
  string s = boost::lexical_cast<string>(k);
  cout << s << endl;
  OMKey k2 = boost::lexical_cast<OMKey>(s);
  ENSURE_EQUAL(k, k2);

  // OMKey goes round-trip (with PMT number)
  k = OMKey(3,14,15);
  s = boost::lexical_cast<string>(k);
  cout << s << endl;
  k2 = boost::lexical_cast<OMKey>(s);
  ENSURE_EQUAL(k, k2);

  k = OMKey(-13,0);
  s = boost::lexical_cast<string>(k);
  k2 = boost::lexical_cast<OMKey>(s);
  ENSURE_EQUAL(k, k2);
}
