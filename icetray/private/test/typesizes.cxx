#include <I3Test.h>
#include <boost/preprocessor/stringize.hpp>
#include <iostream>

TEST_GROUP(type_sizes);


#define DO(t) std::cout << BOOST_PP_STRINGIZE(t) << " size= " << sizeof(t) << "\n";

TEST(show)
{
  DO(bool);
  DO(unsigned char);
  DO(char);
  DO(unsigned short);
  DO(short);
  DO(unsigned int);
  DO(int);
  DO(unsigned long);
  DO(long);
  DO(unsigned long long);
  DO(long long);
  DO(float);
  DO(double);
}

