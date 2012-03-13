/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3LoggingTest.cxx 15877 2006-02-16 14:00:11Z troy $

    @version $Revision: 1.2 $
    @date $Date: 2006-02-16 15:00:11 +0100 (Thu, 16 Feb 2006) $
    @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Context.h>
#include <icetray/OMKey.h>
#include <icetray/lexical_casts.h>
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

  k = OMKey(-13,0);
  s = boost::lexical_cast<string>(k);
  k2 = boost::lexical_cast<OMKey>(s);
  ENSURE_EQUAL(k, k2);
}

TEST(vector_OMKey_out_only)
{
  vector<OMKey> vomk;
  vomk.push_back(OMKey(1,2,0));
  vomk.push_back(OMKey(666,12,0));
  vomk.push_back(OMKey(0,0,0));
  vomk.push_back(OMKey(-1,1,0));

  string s = boost::lexical_cast<string>(vomk);
  log_debug("vector<OMKey> s == '%s'", s.c_str());

  ENSURE_EQUAL(s, "[OMKey(1,2,0), OMKey(666,12,0), OMKey(0,0,0), OMKey(-1,1,0)]");

}
