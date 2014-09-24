#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Int.h>

TEST_GROUP(I3Int);

TEST(default_constructor) {
  I3Int foo;
  ENSURE(foo.value == 0, "Default constructor did not set value to 0");
}

TEST(constructor_from_int) {
  I3Int foo(12);

  // barf if foo != 12
  ENSURE(foo.value == 12, "Constructor from int failed");
}

TEST(constructor_from_I3Int) {
  I3Int foo(12);
  I3Int bar(foo);
  ENSURE(bar.value == 12, "I3Int copy constructor failed");
}

TEST(assignment_operator) {
  I3Int foo = 12;
  I3Int bar = 10;
  bar = foo;
  
  // barf if bar != 12
  ENSURE(bar.value == 12, "Whoa! Assignment failed! bar.value should be 12!");
}


TEST(equal_operator) {
  I3Int foo(12);
  I3Int bar(12);
  ENSURE(foo == bar, "I3Int operator== failed");
  bar = 13;
  ENSURE(!(foo == bar), "I3Int ! operator== failed");
}

TEST(not_equal_operator) {
  I3Int foo(12);
  I3Int bar(13);
  ENSURE(foo != bar, "I3Int operator!= failed");
  bar = 12;
  ENSURE(!(foo != bar), "I3Int ! operator!= failed");
}

TEST(less_than_operator) {
  I3Int foo(12);
  I3Int bar(13);
  ENSURE(foo < bar, "I3Int operator< failed");
  bar = 12;
  ENSURE(!(foo < bar), "I3Int ! operator< failed when equal");
  bar = 11;
  ENSURE(!(foo < bar), "I3Int ! operator< failed");
}

TEST(less_than_equal_operator) {
  I3Int foo(12);
  I3Int bar(13);
  ENSURE(foo <= bar, "I3Int operator<= failed");
  bar = 12;
  ENSURE(foo <= bar, "I3Int operator<= failed when equal");
  bar = 11;
  ENSURE(!(foo <= bar), "I3Int ! operator<= failed");
}

TEST(greater_than_operator) {
  I3Int foo(12);
  I3Int bar(13);
  ENSURE(bar > foo, "I3Int operator> failed");
  bar = 12;
  ENSURE(!(bar > foo), "I3Int ! operator> failed when equal");
  bar = 11;
  ENSURE(!(bar > foo), "I3Int ! operator> failed");
}

TEST(greater_than_equal_operator) {
  I3Int foo(12);
  I3Int bar(13);
  ENSURE(bar >= foo, "I3Int operator>= failed");
  bar = 12;
  ENSURE(bar >= foo, "I3Int operator>= failed when equal");
  bar = 11;
  ENSURE(!(bar >= foo), "I3Int ! operator>= failed");
}
