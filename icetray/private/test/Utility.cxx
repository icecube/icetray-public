/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: ServicesAtDestruction.cxx 5468 2005-03-30 14:48:26Z pretz $
 *
 * @version $Revision: 1.3 $
 * @date $Date: 2005-03-30 16:48:26 +0200 (Wed, 30 Mar 2005) $
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <I3Test.h>
#include <boost/preprocessor/stringize.hpp>
#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

TEST_GROUP(Utility);

TEST(stlfilt)
{
  ENSURE_EQUAL(stlfilt("foo"), "foo");
  ENSURE_EQUAL(stlfilt("std::foo"), "foo");
  ENSURE_EQUAL(stlfilt("std::vector<donk, std::allocator<donk> >"), "vector<donk>");
  ENSURE_EQUAL(stlfilt("std::map<Key, Value, less<Key>, allocator<pair<const Key, Value> > >"),
	       "map<Key, Value>");
}


