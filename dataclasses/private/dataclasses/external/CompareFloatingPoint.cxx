#include <dataclasses/external/CompareFloatingPoint.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#if defined(__vax__) || (defined(__alpha__) && !(defined(___IEEE_FP) || defined(__IEEE_FLOAT)))
#error Non-IEEE 754 compliant floating point
#endif

// Function to print a number and its representation, in hex and decimal
template <typename F, typename I>
inline std::string ToStringImpl(F f, I offset )
{
  const int PREC(2 *sizeof(I));
  I fint;
  memcpy(&fint, &f, sizeof(I));
  fint += offset;
  std::stringstream s;
  s<< " | " << std::showpos << std::setw(PREC) << std::setprecision(PREC) << f << " "
   << " | " << std::right << "0x" << std::hex << std::uppercase 
   << std::setw(PREC) << std::setfill('0') << fint << " "
   << " | " << std::dec << std::setfill(' ') << std::setw(PREC) << f << " "
   << " | " << offset  << " | "
   << std::endl;
  memcpy(&f, &fint, sizeof(I));
  return s.str();
}

std::string CompareFloatingPoint::ToString(double f, int64_t offset ){
  return ToStringImpl<double,int64_t>(f,offset);
}

std::string CompareFloatingPoint::ToString(float f, int32_t offset ){
  return ToStringImpl<float,int32_t>(f,offset);
}
