/**
    copyright  (C) 2004
    the icecube collaboration

*/

#include <I3Test.h>

#include <icetray/OMKey.h>
#include <dataclasses/I3Vector.h>

#include <iostream>
#include <string>
#include <set>

using namespace std;

TEST_GROUP(I3Vector);

TEST(i3vector_to_set_and_back)
{
  I3Vector<OMKey> keyvec;

  // I3Vector contains six omkeys, three of which are duplicates.

  keyvec.push_back(OMKey(0,0));
  keyvec.push_back(OMKey(1,1));
  keyvec.push_back(OMKey(2,2));
  keyvec.push_back(OMKey(1,1));
  keyvec.push_back(OMKey(2,2));
  keyvec.push_back(OMKey(1,1));
  unsigned int expected_size = 6;
  unsigned int expected_size2 = 3;

  ENSURE_EQUAL(keyvec.size(), expected_size);

  // make a std::set

  set<OMKey> keyset(keyvec.begin(), keyvec.end());

  ENSURE_EQUAL(keyset.size(), expected_size2);

  // make a std::vector from the std::set
  vector<OMKey> keyvec_2(keyset.begin(), keyset.end());
  ENSURE_EQUAL(keyvec_2.size(), expected_size2);

  // make an I3Vector from the std::set
  I3Vector<OMKey> keyvec_3(keyset.begin(), keyset.end());
  ENSURE_EQUAL(keyvec_3.size(), expected_size2);



}

