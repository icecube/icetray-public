#include <iostream>
#include <fftw3.h>
#include <math.h>

#include <dataclasses/fft/FFTWPlan.h>
#include <icetray/I3Logging.h>

const unsigned int fft::GetNCFromNR(unsigned int nReal) {return nReal / 2 + 1;}
const unsigned int fft::GetNRFromNC(unsigned int nComp) {return (nComp - 1) * 2;}

//Reset the variables
void FFTWPlan::SetToNull() {
  thePlan_ = NULL;
  inN_ = outN_ = -1;
  inr_ = outr_ = NULL;
  inc_ = outc_ = NULL;
  planSet_ = false;
  isExecuted_ = false;
}

//Safely unallocate using fftw_free
void FFTWPlan::Free() {

  if (thePlan_ != NULL)
    fftw_destroy_plan(thePlan_);

  if (inr_ != NULL)
    fftw_free(inr_);
  if (outr_ != NULL)
    fftw_free(outr_);
  if (inc_ != NULL)
    fftw_free(inc_);
  if (outc_ != NULL)
    fftw_free(outc_);

  SetToNull();
}

//Constructor
FFTWPlan::FFTWPlan(unsigned int inN, fft::FFTType type, int sign, unsigned int flag) {
  SetToNull();

  if (inN == 0) {
    return;
  }

  inN_ = inN;
  theType_ = type;

  if (type == fft::eC2C) {
    outN_ = inN_;

    inc_ = fftw_alloc_complex(inN_);
    outc_ = fftw_alloc_complex(outN_);

    thePlan_ = fftw_plan_dft_1d(inN, inc_, outc_, sign, flag);
    planSet_ = true;
  } else if (type == fft::eC2R) {
    outN_ = fft::GetNRFromNC(inN_);

    inc_ = fftw_alloc_complex(inN_);
    outr_ = fftw_alloc_real(outN_);

    thePlan_ = fftw_plan_dft_c2r_1d(outN_, inc_, outr_, flag);
    planSet_ = true;
  } else if (type == fft::eR2C) {
    outN_ = fft::GetNCFromNR(inN_);

    inr_ = fftw_alloc_real(inN_);
    outc_ = fftw_alloc_complex(outN_);

    thePlan_ = fftw_plan_dft_r2c_1d(inN_, inr_, outc_, flag);
    planSet_ = true;
  } else {
    log_fatal("The FFT type you chose has not been implemented.");
    planSet_ = false;
  }
}

//Copy the REAL data into the plan
void FFTWPlan::CopyIntoPlan(double *arr, unsigned int n) {
  isExecuted_ = false;

  if (n != inN_) {
    log_fatal("I cannot copy this vector in, Boss. It is size %d and I am expecting %d", n, inN_);
  }

  for (unsigned int i = 0; i < n; i++) {
    inr_[i] = arr[i];
  }
}

//Copy the COMPLEX data into the plan
void FFTWPlan::CopyIntoPlanC(double arr[][2], unsigned int n) {
  isExecuted_ = false;

  if (n != inN_) {
    log_fatal("I cannot copy this vector in, Boss. It is size %d and I am expecting %d", n, inN_);
  }

  for (unsigned int i = 0; i < n; i++) {
    for (unsigned int ireal = 0; ireal < 2; ireal++)
      inc_[i][ireal] = arr[i][ireal];
  }
}

//Copy the COMPLEX data into the plan
void FFTWPlan::CopyIntoPlanC(std::complex<double> *arr, unsigned int n) {
  isExecuted_ = false;

  if (n != inN_) {
    log_fatal("I cannot copy this vector in, Boss. It is size %d and I am expecting %d", n, inN_);
  }

  for (unsigned int i = 0; i < n; i++) {
    inc_[i][0] = arr[i].real();
    inc_[i][1] = arr[i].imag();
  }
}

//Copy the REAL data from the plan
void FFTWPlan::CopyOutOfPlan(double *arr, bool norm) {
  ExecutePlan();

  if (fft::eC2R != theType_) {
    log_fatal("Asking for the real output of a complex FFT. I'll stop you before you segfault.");
  }

  for (unsigned int i = 0; i < outN_; i++) {
    arr[i] = outr_[i] * (norm ? sqrt(outN_) : 1.);
  }
}

//Copy the COMPLEX data from the plan
void FFTWPlan::CopyOutOfPlanC(double arr[][2], bool norm) {
  ExecutePlan();

  for (unsigned int i = 0; i < outN_; i++) {
    for (unsigned int ireal = 0; ireal < 2; ireal++)
      arr[i][ireal] = outc_[i][ireal] * (norm ? sqrt(inN_) : 1.);
  }
}

//Copy the COMPLEX data from the plan
void FFTWPlan::CopyOutOfPlanC(std::complex<double> *arr, bool norm) {
  ExecutePlan();

  for (unsigned int i = 0; i < outN_; i++) {
    arr[i] = std::complex<double>(outc_[i][0], outc_[i][1]) * (norm ? sqrt(inN_) : 1.);
  }
}

//Execute the transform
void FFTWPlan::ExecutePlan() {
  if (! isExecuted_) {
    if (!planSet_) {
      log_fatal("Cannot get results because the plan is not set!");
    }
    fftw_execute(thePlan_);
    isExecuted_ = true;
  }
}

//Print the result to the terminal
void FFTWPlan::PrintPlan() const {
  if (theType_ == fft::eC2C || theType_ == fft::eR2C) {
    for (unsigned int i = 0; i < outN_; i++) {
      std::cerr << '(' << outc_[i][0] << ',' << outc_[i][1] << ')' << std::endl;
    }
  } else if (theType_ == fft::eC2R) {
    for (unsigned int i = 0; i < outN_; i++) {
      std::cerr << outr_[i] << std::endl;
    }
  }
}
