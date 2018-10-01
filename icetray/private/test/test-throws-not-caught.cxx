#include <I3Test.h>
#include <boost/shared_ptr.hpp>

TEST_GROUP(test_throws_not_caught);

void throw_something()
{
  try {
    ENSURE(false, "this should show up with a nice line number");
  } catch (const std::exception& e) {
    ENSURE(false, "this should *not* get caught here...");
  } catch (const I3Test::I3TestException& e) {
    // yes we should be here.
  }
}

TEST(test_throws_dont_derive_from_std_exception)
{
  throw_something();
}

