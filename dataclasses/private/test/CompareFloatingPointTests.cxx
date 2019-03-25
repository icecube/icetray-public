#include <I3Test.h>
#include <dataclasses/external/CompareFloatingPoint.h>

using namespace std;

// Test CompareAsInt Functions
TEST_GROUP(CompareAsInt);

using namespace CompareFloatingPoint;

TEST(size){
  ENSURE( sizeof(float) == sizeof(int32_t) );
  ENSURE( sizeof(double) == sizeof(int64_t) );
}

// Function to test the TestCompareFinal function
bool TestCompareFinal(float A, float B, bool expectedResult, int32_t maxUlps = 10)
{
  bool result = Compare(A, B, maxUlps);
  if (result != expectedResult){
    return false;
  }else return true;
}

// Function to test the TestCompareFinal function
bool TestCompareFinal(double A, double B, bool expectedResult, int64_t maxUlps = 10)
{
  bool result = Compare(A, B, maxUlps);
  if (result != expectedResult){
    return false;
  }else return true;
}

void PrintNumbers()
{
    // This function just prints some numbers, as floats, hex int, and decimal int for
    // incorporation into my article on comparing floats as ints.

  float f = 2.0;
  cerr<<"Offset to "<<f<<" runs from -2 to 2"<<endl;
  for (int32_t i = -2; i <= 2; ++i)
    cerr << ToString(f, i) <<endl;

  cerr<<"Zero plus 0 should still be 0"<<endl
      <<ToString((float)0.0)
      <<endl;

  cerr<<"Zero plus -0 should also be 0"<<endl
      <<ToString((float)0.0, 0x80000000)<<endl;
  
  cerr<<"Offset to 0.0 runs from 3 to -3"<<endl;
  for (int32_t i = 3; i >= 0; --i)
    cerr<<ToString( (float)0.0, i ) <<endl;
  for (int32_t i = 0; i <= 3; ++i)
    cerr<<ToString( (float)0.0, i + 0x80000000 )<<endl;
 
  double d = 2.0;
  cerr<<"Offset to "<<f<<" runs from -2 to 2"<<endl;
  for (int64_t i = -2; i <= 2; ++i)
    cerr << ToString(d, i) << endl;

  cerr<<"Zero plus 0 should still be 0"<<endl
      <<ToString((double)0.0) <<endl;

  cerr<<"Zero plus -0 should also be 0"<<endl
      <<ToString((double)0.0, 0x8000000000000000LL)<<endl;
  
  cerr<<"Offset to 0.0 runs from 3 to -3"<<endl;
  for (int64_t i = 3; i >= 0; --i)
    cerr<<ToString( (double)0.0, i ) <<endl;
  for (int64_t i = 0; i <= 3; ++i)
    cerr<<ToString( (double)0.0, i + 0x8000000000000000LL )<<endl;
}

TEST(compare_float_as_int32_t){

  PrintNumbers();

  float zero1(0);
  float zero2(0);

  // Create various special numbers
  float negativeZero;
  // Initialize negativeZero with its integer representation
  int stub;
  stub = 0x80000000;
  memcpy(&negativeZero, &stub, sizeof(stub));
  // Create a NAN
  float nan1 = sqrt(-1.0f);
  // Create a NAN a different way - should give the same NAN on
  // Intel chips.
  float nan2 = zero1 / zero2;
  // Create an infinity
  float inf = 1 / zero1;
  // Create a NAN a third way - should give the same NAN on
  // Intel chips.
  float nan3 = inf - inf;
  // Copy one of the NANs and modify its representation.
  // This will still give a NAN, just a different one.
  float nan4 = nan3;
  memcpy(&stub, &nan4, sizeof(stub));
  stub += 1;
  memcpy(&nan4, &stub, sizeof(stub));
  
  // Create a denormal by starting with zero and incrementing
  // the integer representation.
  float smallestDenormal = 0;
  memcpy(&stub, &smallestDenormal, sizeof(stub));
  stub += 1;
  memcpy(&smallestDenormal, &stub, sizeof(stub));
  
  cerr<<"negativeZero " <<endl;
  cerr<<ToString(negativeZero);
  cerr<<"nan1 " <<endl;
  cerr<<ToString(nan1);
  cerr<<"nan2 " <<endl;
  cerr<<ToString(nan2);
  cerr<<"inf " <<endl;
  cerr<<ToString(inf);
  cerr<<"nan3 " <<endl;
  cerr<<ToString(nan3);
  cerr<<"nan4 " <<endl;
  cerr<<ToString(nan4);
  cerr<<"smallestDenormal " <<endl;
  cerr<<ToString(smallestDenormal);

  // The first set of tests check things that any self-respecting
  // comparison function should agree upon.
  
  // Make sure that zero and negativeZero compare as equal.
  ENSURE( TestCompareFinal( zero1, negativeZero, true ) ) ;
  
  // Make sure that nearby numbers compare as equal.
  ENSURE( TestCompareFinal( (float)2.0, (float)1.9999999f, true ) ) ;

  // Make sure that slightly more distant numbers compare as equal.
  ENSURE( TestCompareFinal( (float)2.0, (float)1.9999995f, true ) ) ;
  
  // Make sure the results are the same with parameters reversed.
  ENSURE( TestCompareFinal( (float)1.9999995f, (float)2.0, true ) ) ;
  
  // Make sure that even more distant numbers don't compare as equal.
  ENSURE( TestCompareFinal( (float)2.0, (float)1.999995f, false ) ) ;
  
  
  // The next set of tests check things where the correct answer isn't
  // as obvious or important. Some of these tests check for cases that
  // are rare or can easily be avoided. Some of them check for cases
  // where the behavior of the 2sComplement function is arguably better
  // than the behavior of the fussier Final function.
  
  // Test whether FLT_MAX and infinity (representationally adjacent)
  // compare as equal.
  ENSURE( TestCompareFinal( FLT_MAX, inf, false ) ) ;
  
  // Test whether a NAN compares as equal to itself.
  ENSURE( TestCompareFinal( nan2, nan2, false ) ) ;
  
  // Test whether a NAN compares as equal to a different NAN.
  ENSURE( TestCompareFinal( nan2, nan1, false ) ) ;

  ENSURE( TestCompareFinal( nan2, nan3, false ) ) ;
  
  // Test whether tiny numbers of opposite signs compare as equal.
  // I know this seems odd, but they don't.  It's difficult to do this
  // consistently across the spectrum, so I'm going to keep the old
  // behavior where *any* number of opposite sign compare not equal.
  ENSURE( TestCompareFinal( smallestDenormal, -smallestDenormal, false ) ) ;

  
  ENSURE( TestCompareFinal( smallestDenormal, 0, true ) ) ;
  ENSURE( TestCompareFinal( -smallestDenormal, negativeZero, true ) ) ;

  cerr<<ToString(nan1); 
  cerr<<ToString(nan2);
  cerr<<ToString(nan3);
  cerr<<ToString(nan4);
 
}

TEST(compare_double_as_int64_t){
  
  double zero1(0);
  double zero2(0);
  uint64_t stub;

 // Create various special numbers
  double negativeZeroD;
  // Initialize negativeZero with its integer representation
  stub = 0x8000000000000000LL;
  memcpy(&negativeZeroD, &stub, sizeof(stub));
  // Create a NAN
  double nan1D = sqrt(-1.0f);
  // Create a NAN a different way - should give the same NAN on
  // Intel chips.
  double nan2D = (double)zero1 / (double)zero2;
  // Create an infinity
  double infD = 1 / (double)zero1;
  // Create a NAN a third way - should give the same NAN on
  // Intel chips.
  double nan3D = infD - infD;
  // Copy one of the NANs and modify its representation.
  // This will still give a NAN, just a different one.
  double nan4D = nan3D;
  memcpy(&stub, &nan4D, sizeof(stub));
  stub += 1;
  memcpy(&nan4D, &stub, sizeof(stub));

  // Create a denormal by starting with zero and incrementing
  // the integer representation.
  double smallestDenormalD = 0;
  memcpy(&stub, &smallestDenormalD, sizeof(stub));
  stub += 1;
  memcpy(&smallestDenormalD, &stub, sizeof(stub));


  cerr<<"negativeZeroD " <<endl;
  cerr<<ToString(negativeZeroD);
  cerr<<"nan1D " <<endl;
  cerr<<ToString(nan1D);
  cerr<<"nan2D " <<endl;
  cerr<<ToString(nan2D);
  cerr<<"infD " <<endl;
  cerr<<ToString(infD);
  cerr<<"nan3D " <<endl;
  cerr<<ToString(nan3D);
  cerr<<"nan4D " <<endl;
  cerr<<ToString(nan4D);
  cerr<<"smallestDenormalD " <<endl;
  cerr<<ToString(smallestDenormalD);

  // now test the doubles
  double zeroD(0);
  // Make sure that zero and negativeZero compare as equal.
  ENSURE( TestCompareFinal( zeroD, negativeZeroD, true ) ) ;
  
  // Make sure that nearby numbers compare as equal.
  ENSURE( TestCompareFinal( (double)2.0, (double)1.9999999f, false ) ) ;
  ENSURE( TestCompareFinal( (double)2.0, (double)1.99999999999999f, true ) ) ;
  
  // Make sure that slightly more distant numbers compare as equal.
  //ENSURE( TestCompareFinal( (double)2.0, (double)1.9999995f, false ) ) ;
  ENSURE( TestCompareFinal( (double)2.0, (double)1.9999995f, false ) ) ;
 
  ENSURE( TestCompareFinal( (double)2.0, (double)1.99999999999995f, true ) ) ;
  
  // Make sure the results are the same with parameters reversed.
  //ENSURE( TestCompareFinal( (double)1.9999995f, (double)2.0, false ) ) ;
  ENSURE( TestCompareFinal( (double)1.99999999999995f, (double)2.0, true ) ) ;
  
  // Make sure that even more distant numbers don't compare as equal.
  //ENSURE( TestCompareFinal( (double)2.0, (double)1.9999995f, false ) ) ;
  ENSURE( TestCompareFinal( (double)2.0, (double)1.9999999999995f, true ) ) ;
  
  
  // The next set of tests check things where the correct answer isn't
  // as obvious or important. Some of these tests check for cases that
  // are rare or can easily be avoided. Some of them check for cases
  // where the behavior of the 2sComplement function is arguably better
  // than the behavior of the fussier Final function.
  
  // Test whether DBL_MAX and infinity (representationally adjacent)
  // compare as equal.
  ENSURE( TestCompareFinal( DBL_MAX, infD, false ) ) ;
  
  ENSURE( std::isinf(infD) );

  cerr<<ToString(nan1D);
  cerr<<ToString(nan2D);
  cerr<<ToString(nan3D);
  cerr<<ToString(nan4D);

  ENSURE( std::isnan(nan1D) );
  ENSURE( std::isnan(nan2D) );
  ENSURE( std::isnan(nan3D) );
  ENSURE( std::isnan(nan4D) );

  // Test whether a NAN compares as equal to itself.
  ENSURE( TestCompareFinal( nan2D, nan2D, false ) ) ;
  
  // Test whether a NAN compares as equal to a different NAN.
  ENSURE( TestCompareFinal( nan2D, nan1D, false ) ) ;

  ENSURE( TestCompareFinal( nan2D, nan3D, false ) ) ;
  
  // Test whether tiny numbers of opposite signs compare as equal.
  ENSURE( TestCompareFinal( smallestDenormalD, -smallestDenormalD, false ) ) ;

  // The smallestDenormal is 1 ULP away from 0, so should compare
  // true when the default distance is 10 ulps.  The smallest
  ENSURE( TestCompareFinal( smallestDenormalD, 0, true) ) ;
  ENSURE( TestCompareFinal( -smallestDenormalD, negativeZeroD, true) ) ;
  
}

