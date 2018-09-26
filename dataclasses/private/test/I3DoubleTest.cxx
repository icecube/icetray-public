#include <I3Test.h>

#include <dataclasses/I3Double.h>

TEST_GROUP(I3Double);

TEST(default_constructor) {
  I3Double foo;
  ENSURE(foo.value == 0.0, "Default constructor did not set value to 0.0");
}

TEST(constructor_from_double) {
  I3Double foo(12.0);

  // barf if foo != 12.0
  ENSURE(foo.value == 12.0, "Constructor from double failed");
}

TEST(constructor_from_I3Double) {
  I3Double foo(12.0);
  I3Double bar(foo);
  ENSURE(bar.value == 12.0, "I3Double copy constructor failed");
}

TEST(assignment_operator) {
  I3Double foo = 12.0;
  I3Double bar = 10.0;
  bar = foo;
  
  // barf if bar != 12.0
  ENSURE(bar.value == 12.0, "Whoa! Assignment failed! bar.value should be 12.0!");
}

TEST(equal_operator) {
  I3Double foo(12.0);
  I3Double bar(12.0);
  ENSURE(foo == bar, "I3Double operator== failed");
  bar = 13.0;
  ENSURE(!(foo == bar), "I3Double ! operator== failed");
}

TEST(not_equal_operator) {
  I3Double foo(12.0);
  I3Double bar(13.0);
  ENSURE(foo != bar, "I3Double operator!= failed");
  bar = 12.0;
  ENSURE(!(foo != bar), "I3Double ! operator!= failed");
}

TEST(less_than_operator) {
  I3Double foo(12.0);
  I3Double bar(13.0);
  ENSURE(foo < bar, "I3Double operator< failed");
  bar = 12.0;
  ENSURE(!(foo < bar), "I3Double ! operator< failed when equal");
  bar = 11.0;
  ENSURE(!(foo < bar), "I3Double ! operator< failed");
}

TEST(less_than_equal_operator) {
  I3Double foo(12.0);
  I3Double bar(13.0);
  ENSURE(foo <= bar, "I3Double operator<= failed");
  bar = 12.0;
  ENSURE(foo <= bar, "I3Double operator<= failed when equal");
  bar = 11.0;
  ENSURE(!(foo <= bar), "I3Double ! operator<= failed");
}

TEST(greater_than_operator) {
  I3Double foo(12.0);
  I3Double bar(13.0);
  ENSURE(bar > foo, "I3Double operator> failed");
  bar = 12.0;
  ENSURE(!(bar > foo), "I3Double ! operator> failed when equal");
  bar = 11.0;
  ENSURE(!(bar > foo), "I3Double ! operator> failed");
}

TEST(greater_than_equal_operator) {
  I3Double foo(12.0);
  I3Double bar(13.0);
  ENSURE(bar >= foo, "I3Double operator>= failed");
  bar = 12.0;
  ENSURE(bar >= foo, "I3Double operator>= failed when equal");
  bar = 11.0;
  ENSURE(!(bar >= foo), "I3Double ! operator>= failed");
}
