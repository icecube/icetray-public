#include <I3Test.h>

#include <dataclasses/I3String.h>

TEST_GROUP(I3String);

TEST(default_constructor) {
  I3String foo;
  ENSURE(foo.value == "", "Default constructor did not set value to empty string");
}

TEST(constructor_from_string) {
  I3String foo("test");

  // barf if foo != test
  ENSURE(foo.value == "test", "Constructor from string failed");
}

TEST(constructor_from_I3String) {
  I3String foo("test");
  I3String bar(foo);
  ENSURE(bar.value == "test", "I3String copy constructor failed");
}

TEST(assignment_operator) {
  I3String foo = std::string("test");
  I3String bar = std::string("tester");
  bar = foo;

  // barf if bar != test
  ENSURE(bar.value == "test", "Whoa! Assignment failed! bar.value should be test!");
}

TEST(equal_operator) {
  I3String foo("test");
  I3String bar("test");
  ENSURE(foo == bar, "I3String operator== failed");
  bar = std::string("tester");
  ENSURE(!(foo == bar), "I3String ! operator== failed");
}

TEST(not_equal_operator) {
  I3String foo("test");
  I3String bar("tester");
  ENSURE(foo != bar, "I3String operator!= failed");
  bar = std::string("test");
  ENSURE(!(foo != bar), "I3String ! operator!= failed");
}

TEST(stream_operator) {
  I3String foo("test");
  std::ostringstream bar;
  bar << foo;
  ENSURE(("I3String(\"test\")" == bar.str()), "I3String operator<< failed");
  
}
