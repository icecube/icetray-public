/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: LexicalCasts.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $

    @version $Revision: 165886 $
    @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $
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
  vomk.push_back(OMKey(1,2));
  vomk.push_back(OMKey(666,12));
  vomk.push_back(OMKey(0,0));
  vomk.push_back(OMKey(-1,1));

  string s = boost::lexical_cast<string>(vomk);
  log_debug("vector<OMKey> s == '%s'", s.c_str());

  ENSURE_EQUAL(s, "[OMKey(1,2), OMKey(666,12), OMKey(0,0), OMKey(-1,1)]");

}
