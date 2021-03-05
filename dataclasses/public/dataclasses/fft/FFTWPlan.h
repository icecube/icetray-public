////////////////////////////////////////////////////////////
//Author: Alan Coleman
//Wrapper class to handle all calls to fftw3
////////////////////////////////////////////////////////////
//Warning, fftw3 performs unnormalized FFTs
//You can request a normalized output in CopyOutOfPlan
//It is not recommended that you use this class to do FFTs, instead
//use the wrapper class FFTDataContainer


#ifndef RADCUBE_UTIL_FFTWPLAN_H
#define RADCUBE_UTIL_FFTWPLAN_H

#include <fftw3.h>
#include <vector>
#include <complex>

namespace fft {

enum FFTType {
  eR2C = 0,  //Real to complex transform
  eC2R = 1,  //Complex to real transform
  eC2C = 2,   //Complex to complex transform
  eFFTTypeCount = 3 //This is for debugging and printing, no pybinding
};

//Simple functions to get the number of bins in a Real <---> Complex transform
const unsigned int GetNCFromNR(unsigned int nReal);
const unsigned int GetNRFromNC(unsigned int nComp);

} //fft

class FFTWPlan {
 public:
  FFTWPlan() {SetToNull();}
  FFTWPlan(unsigned int inN, fft::FFTType type, int sign = FFTW_FORWARD, unsigned int flag = FFTW_MEASURE);
  ~FFTWPlan() {Free();}

  fft::FFTType GetType() {return theType_;}

  /////Setters and getters of the information (careful, un-normalized)
  void CopyIntoPlan(double * arr, unsigned int n);
  void CopyIntoPlanC(double arr[][2], unsigned int n);
  void CopyIntoPlanC(std::complex<double>* arr, unsigned int n);
  void CopyOutOfPlan(double *arr, bool norm = false);
  void CopyOutOfPlanC(double arr[][2], bool norm = false);
  void CopyOutOfPlanC(std::complex<double>* arr, bool norm = false);

  void ExecutePlan();  //Execute the transform

  void PrintPlan() const;  //Print the result to terminal

 private:
  void SetToNull(); //Reset the variables
  void Free(); //Safely unallocate using fftw_free

  fftw_plan thePlan_;
  bool planSet_;
  bool isExecuted_;

  fft::FFTType theType_;

  unsigned int inN_;
  unsigned int outN_;

  fftw_complex *inc_, *outc_;
  double *inr_, *outr_;

};

#endif
