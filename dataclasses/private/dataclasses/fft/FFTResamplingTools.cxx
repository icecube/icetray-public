#include <dataclasses/fft/FFTResamplingTools.h>

boost::rational<long> fft::GetRationalNumberFromDecimal(double decimal, const bool secondCall) {

  const double abortprecision = 1e-5; //Number of sig figs
  const double originalDecimal = decimal;
  long denominator = 1;
  double decimalInteger; //Store the integer part of decimal number here

  //Get the number stored to the correct number of sig figs
  while (std::modf(decimal, &decimalInteger) / decimal > abortprecision) {
    decimal *= 10.0;
    denominator *= 10;
  }

  //If this is the second call, return the value now
  if (secondCall)
    return boost::rational<long>(denominator, long(decimal + 0.5));

  //Calculate the current rational number as well as the result using the inverse
  const boost::rational<long> firstResult = boost::rational<long>(long(decimal + 0.5), denominator);
  const boost::rational<long> secondResult = GetRationalNumberFromDecimal(1.0 / originalDecimal, true);

  //Return value with smaller denominator
  return (firstResult.denominator() < secondResult.denominator()) ? firstResult : secondResult;
}