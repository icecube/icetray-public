/*
CompareAsInt.cpp - various functions to compare floating point numbers using
integer operations, other functions to test these comparisons, and other
functions to print out floating point numbers and their representation.
This comes from here : http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
We'll see this a lot below so I'll include a snippet from the website.

The IEEE float and double formats were designed so that the numbers are lexicographically orderedâ,
which in the words of IEEE architect William Kahan means "if two floating-point numbers in the same 
format are ordered ( say x < y ), then they are ordered the same way when their bits are reinterpreted 
as Sign-Magnitude integers."
 
This means that if we take two floats in memory, interpret their bit pattern as integers, and compare 
them, we can tell which is larger, without doing a floating point comparison. In the C/C++ language 
this comparison looks like this:

if (*(int*)&f1 < *(int*)&f2)

This charming syntax means take the address of f1, treat it as an integer pointer, and dereference it. 
All those pointer operations look expensive, but they basically all cancel out and just mean "treat f1 
as an integer". Since we apply the same syntax to f2 the whole line means compare f1 and f2, 
using their in-memory representations interpreted as integers instead of floats.

NB:  Another thing you'll see is "Ulps" which stands for "Units in the Last Place."
*/
#ifndef COMPARE_FLOATING_POINT
#define COMPARE_FLOATING_POINT

#include <float.h>
#include <cmath>
#include <stdint.h>
#include <cstring>
#include <string>
#include <cstdlib>

namespace CompareFloatingPoint{
  
  // Function to print a number and its representation, in hex and decimal
  std::string ToString(double f, int64_t offset = 0);
  std::string ToString(float f, int32_t offset = 0);
  
  bool Compare(float A, float B, int32_t maxUlps = 10);
  bool Compare(double A, double B, int64_t maxUlps = 10);
  
  bool Compare_NanEqual(double A, double B);
  
  int32_t Sign(float);
  int64_t Sign(double);
  
}//end namespace CompareFloatingPoint

inline int32_t CompareFloatingPoint::Sign(float A){
  // The sign bit of a number is the high bit.
  int32_t aInt;
  memcpy( &aInt, &A, sizeof(int32_t) );  
  return aInt & 0x80000000;
}

inline int64_t CompareFloatingPoint::Sign(double A){
  // The sign bit of a number is the high bit.
  int64_t aInt;
  memcpy( &aInt, &A, sizeof(int64_t) );
  return aInt & 0x8000000000000000LL;
}

// This is the 'final' version of the AlmostEqualUlps function.
// The optional checks are included for completeness, but in many
// cases they are not necessary, or even not desirable.
inline bool CompareFloatingPoint::Compare(float A, float B, int32_t maxUlps){
  // There are several optional checks that you can do, depending
  // on what behavior you want from your floating point comparisons.
  // These checks should not be necessary and they are included
  // mainly for completeness.
  
  // If A or B are infinity (positive or negative) then
  // only return true if they are exactly equal to each other -
  // that is, if they are both infinities of the same sign.
  // This check is only needed if you will be generating
  // infinities and you don't want them 'close' to numbers
  // near FLT_MAX.
  if (std::isinf(A) || std::isinf(B))
    return A == B;
  
  // If A or B are a NAN, return false. NANs are equal to nothing,
  // not even themselves.
  // This check is only needed if you will be generating NANs
  // and you use a maxUlps greater than 4 million or you want to
  // ensure that a NAN does not equal itself.
  if (std::isnan(A) || std::isnan(B))
    return false;

  if (Sign(A) != Sign(B))
    return A == B;

  if(std::isfinite(A) && std::isfinite(B)){
    int32_t aInt;
    memcpy( &aInt, &A, sizeof(int32_t) );
    // Make aInt lexicographically ordered as a twos-complement int
    if (aInt < 0)
      aInt = 0x80000000 - aInt;
    // Make bInt lexicographically ordered as a twos-complement int
    int32_t bInt;
    memcpy( &bInt, &B, sizeof(int32_t) );
    if (bInt < 0)
      bInt = 0x80000000 - bInt;
  
    // Now we can compare aInt and bInt to find out how far apart A and B
    // are.
    int32_t intDiff = abs(aInt - bInt);
    return bool{intDiff <= maxUlps};
  }
  
  // At this point we should be comparing 0 to a 'normal'
  return A == B;
}

// This is the 'final' version of the AlmostEqualUlps function.
// The optional checks are included for completeness, but in many
// cases they are not necessary, or even not desirable.
inline bool CompareFloatingPoint::Compare(double A, double B, int64_t maxUlps){
  // There are several optional checks that you can do, depending
  // on what behavior you want from your floating point comparisons.
  // These checks should not be necessary and they are included
  // mainly for completeness.

  // If A or B are infinity (positive or negative) then
  // only return true if they are exactly equal to each other -
  // that is, if they are both infinities of the same sign.
  // This check is only needed if you will be generating
  // infinities and you don't want them 'close' to numbers
  // near FLT_MAX.
  if (std::isinf(A) || std::isinf(B))
    return A == B;
  
  // If A or B are a NAN, return false. NANs are equal to nothing,
  // not even themselves.
  // This check is only needed if you will be generating NANs
  // and you use a maxUlps greater than 4 million or you want to
  // ensure that a NAN does not equal itself.
  if (std::isnan(A) || std::isnan(B))
    return false;

  if (Sign(A) != Sign(B))
    return A == B;

  if(std::isfinite(A) && std::isfinite(B)){
    int64_t aLong;
    memcpy( &aLong, &A, sizeof(int64_t) );
    // Make aLong lexicographically ordered as a twos-complement long
    if (aLong < 0)
      aLong = 0x8000000000000000LL - aLong;
    // Make bLong lexicographically ordered as a twos-complement long
    int64_t bLong;
    memcpy( &bLong, &B, sizeof(int64_t) );
    if (bLong < 0)
      bLong = 0x8000000000000000LL - bLong;
  
    // Now we can compare aLong and bLong to find out how far apart A and B are.
    int64_t longDiff = llabs(aLong - bLong);
    return bool{longDiff <= maxUlps};
  }

  return A == B;

}

inline bool CompareFloatingPoint::Compare_NanEqual(double A, double B)
{
  return ((std::isnan(A) && std::isnan(B)) || A == B);        
}
  
#endif
