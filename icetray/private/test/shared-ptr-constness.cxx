#include <I3Test.h>
#include <boost/shared_ptr.hpp>

TEST_GROUP(shared_ptr_constness)

struct S
{
  int i;
};

TEST(pointer_to_const_S)
{
  // spcs == shared pointer to const S
  boost::shared_ptr<const S> spcs(new S), spcs2(new S);
  spcs = spcs2; 
  //  spcs->i = 4; // should be error
  spcs = boost::shared_ptr<const S>(new S);
}

TEST(const_pointer_to_S)
{
  // csps == const shared pointer to S
  const boost::shared_ptr<S> csps(new S), csps2(new S); 
  csps->i = 4; // OK, pointer is const, not S

  // error: can't assign to const pointer; is only a
  // warning.  should be an error.
  //  csps = csps2; 
}

TEST(const_pointer_to_const_S)
{
  const boost::shared_ptr<const S> cspcs(new S), cspcs2(new S);
  //  cspcs->i = 4; // error S is const
  //  cspcs = cspcs2; // error, shared_ptr is const
}
