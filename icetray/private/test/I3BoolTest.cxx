#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>

TEST_GROUP(I3Bool);

TEST(default_constructor) {
  I3Bool foo;
  if (foo.value)
    FAIL("Default constructor returned true!");
}

TEST(constructor_from_bool) {
  I3Bool foo(true);

  // barf if foo != true
  ENSURE(foo.value, "Constructor from bool returned true!");
}

TEST(constructor_from_I3Bool) {
  I3Bool foo;
  I3Bool bar(foo);
  if (bar.value)
    FAIL("I3Bool constructor returned true!");
}

TEST(assignment_opperator) {
  I3Bool foo = true;
  I3Bool bar = false;
  bar = foo;
  
  // barf is bar != true
  ENSURE(bar.value, "Whoa! Assignment failed! bar.value should be true!");
}

TEST(equal_operator) {
  I3Bool foo(true);
  I3Bool bar(true);
  ENSURE(foo == bar, "I3Bool operator== failed");
  bar = false;
  ENSURE(!(foo == bar), "I3Bool ! operator== failed");
}

TEST(not_equal_operator) {
  I3Bool foo(true);
  I3Bool bar(false);
  ENSURE(foo != bar, "I3Bool operator!= failed");
  bar = true;
  ENSURE(!(foo != bar), "I3Bool ! operator!= failed");
}
