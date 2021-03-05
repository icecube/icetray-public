#include <dataclasses/fft/FFTHilbertEnvelope.h>
#include <dataclasses/fft/FFTDataContainer.h>
#include <dataclasses/fft/FFTWPlan.h>

/////////////////////////////////////////
////////Time:double, Freq:complex////////
/////////////////////////////////////////

//Wrapper so you can call for FFTDataContainers
AntennaTimeSeries fft::GetHilbertEnvelope(const FFTData &data) {
  return GetHilbertEnvelope(data.GetConstTimeSeries());
}


AntennaTimeSeries fft::GetHilbertEnvelope(const AntennaTimeSeries &inputTimeSeries) {
  if(1 > inputTimeSeries.GetSize()){
    log_warn("Trying to get a hilbert envelope on an empty trace. Something went wrong.");
    return AntennaTimeSeries();
  }

  AntennaSpectrum analytic = GetAnalyticSignal(inputTimeSeries);

  AntennaTimeSeries outTrace = AntennaTimeSeries();
  for (unsigned int ibin = 0; ibin < analytic.GetSize(); ibin++) {
    outTrace.PushBack(std::abs(analytic[ibin]));
  }

  outTrace.SetBinning(inputTimeSeries.GetBinning());
  outTrace.SetOffset(inputTimeSeries.GetOffset());

  return outTrace;
}


//Wrapper so you can call for FFTDataContainers
AntennaSpectrum fft::GetAnalyticSignal(const FFTData &data) {
  return GetAnalyticSignal(data.GetConstTimeSeries());
}


AntennaSpectrum fft::GetAnalyticSignal(const AntennaTimeSeries &inputTimeSeries) {

  const int n = inputTimeSeries.GetSize();
  std::complex<double> timeSeries[n];

  for (int ibin = 0; ibin < n; ibin++) {
    timeSeries[ibin] = std::complex<double>(inputTimeSeries[ibin], 0);
  }

  ApplyHilbertTransform(timeSeries, n);

  AntennaSpectrum outTrace;
  for (int ibin = 0; ibin < n; ibin++) {
    const double real = inputTimeSeries[ibin];
    const double imag = std::real(timeSeries[ibin]);
    outTrace.PushBack(std::complex<double>(real, -imag));
  }

  outTrace.SetBinning(inputTimeSeries.GetBinning());
  outTrace.SetOffset(inputTimeSeries.GetOffset());

  return outTrace;
}


void fft::ApplyHilbertTransform(std::complex<double> *timeSeries, const int n) {
  //Performs the Hilbert transform to the times series which is an array of
  //complex numbers with (real_i, 0) for all entries
  //Transform is (- I sgn(w_i)) * F(w_i)

  //Perform the FFT
  FFTWPlan forwardPlan(n, fft::eC2C, FFTW_FORWARD);
  forwardPlan.CopyIntoPlanC(timeSeries, n);
  forwardPlan.ExecutePlan();
  forwardPlan.CopyOutOfPlanC(timeSeries);

  for (int i = 0; i < n; i++) {
    double a = std::real(timeSeries[i]);
    double b = std::imag(timeSeries[i]);
    if (i < n / 2) { //negative freq
      timeSeries[i] = std::complex<double>(-b, a);
    } else {  //positive freq
      timeSeries[i] = std::complex<double>(b, -a);
    }
  }

  //FFT^-1 back to time domain
  FFTWPlan backwardPlan(n, fft::eC2C, FFTW_BACKWARD);
  backwardPlan.CopyIntoPlanC(timeSeries, n);
  backwardPlan.ExecutePlan();
  backwardPlan.CopyOutOfPlanC(timeSeries);

  for (int ibin = 0; ibin < n; ibin++) {
    timeSeries[ibin] /= double(n); //Normalize
  }
}


double fft::GetHilbertPeakTime(const FFTData &data, double &maxVal) {
  return GetHilbertPeakTime(data.GetConstTimeSeries(), maxVal);
}


double fft::GetHilbertPeakTime(const AntennaTimeSeries &timeSeries, double &maxVal) {
  const double dt = timeSeries.GetBinning();
  const double t0 = timeSeries.GetOffset();

  if (std::isnan(dt) || std::isnan(t0)) {
    log_warn("Trying to get the Hilbert peak time on a trace with uninitialized binning.");
    return 0;
  }

  AntennaTimeSeries hilbert = GetHilbertEnvelope(timeSeries);

  maxVal = 0;
  double tMax = 0;

  for (unsigned int ibin = 0; ibin < hilbert.GetSize(); ibin++) {
    if (hilbert[ibin] > maxVal) {
      maxVal = hilbert[ibin];
      tMax = ibin * dt + t0;
    }
  }//For each spectrum bin

  return tMax;
}

double fft::GetHilbertPeakTime(const FFTData &data) {
  double temp;
  return GetHilbertPeakTime(data, temp);
}

double fft::GetHilbertPeakTime(const AntennaTimeSeries &timeSeries) {
  double temp;
  return GetHilbertPeakTime(timeSeries, temp);
}

double fft::GetHilbertPeakTime(const FFTData3D &data, double &maxVal) {
  return GetHilbertPeakTime(data.GetConstTimeSeries(), maxVal);
}

double fft::GetHilbertPeakTime(const EFieldTimeSeries &timeSeries, double &maxVal) {
  const double dt = timeSeries.GetBinning();
  const double t0 = timeSeries.GetOffset();

  if (std::isnan(dt) || std::isnan(t0)) {
    log_warn("Trying to get the Hilbert peak time on a trace with uninitialized binning.");
    return 0;
  }

  EFieldTimeSeries hilbert = GetHilbertEnvelope(timeSeries);

  maxVal = 0;
  double tMax = 0;

  for (unsigned int ibin = 0; ibin < hilbert.GetSize(); ibin++) {
    const double thisAmp = (hilbert[ibin]).Magnitude();
    if (thisAmp > maxVal) {
      maxVal = thisAmp;
      tMax = ibin * dt + t0;
    }
  }//For each spectrum bin

  return tMax;
}


/////////////////////////////////////////
//Time:I3Position, Freq:I3ComplexVector///
/////////////////////////////////////////


//Wrapper so you can call for FFTDataContainers
EFieldTimeSeries fft::GetHilbertEnvelope(const FFTData3D &data) {
  return GetHilbertEnvelope(data.GetConstTimeSeries());
}


EFieldTimeSeries fft::GetHilbertEnvelope(const EFieldTimeSeries &timeSeries) {
  AntennaTimeSeries xTrace = AntennaTimeSeries();
  AntennaTimeSeries yTrace = AntennaTimeSeries();
  AntennaTimeSeries zTrace = AntennaTimeSeries();

  xTrace.SetBinning(timeSeries.GetBinning());
  yTrace.SetBinning(timeSeries.GetBinning());
  zTrace.SetBinning(timeSeries.GetBinning());

  xTrace.SetOffset(timeSeries.GetOffset());
  yTrace.SetOffset(timeSeries.GetOffset());
  zTrace.SetOffset(timeSeries.GetOffset());

  for (unsigned int ibin = 0; ibin < timeSeries.GetSize(); ibin++) {
    xTrace.PushBack(timeSeries[ibin].GetX());
    yTrace.PushBack(timeSeries[ibin].GetY());
    zTrace.PushBack(timeSeries[ibin].GetZ());
  }

  AntennaSpectrum xAnalytic = GetAnalyticSignal(xTrace);
  AntennaSpectrum yAnalytic = GetAnalyticSignal(yTrace);
  AntennaSpectrum zAnalytic = GetAnalyticSignal(zTrace);


  EFieldTimeSeries outTrace = EFieldTimeSeries();
  for (unsigned int ibin = 0; ibin < xAnalytic.GetSize(); ibin++) {
    I3Position vec;
    vec.SetX(std::abs(xAnalytic[ibin]));
    vec.SetY(std::abs(yAnalytic[ibin]));
    vec.SetZ(std::abs(zAnalytic[ibin]));
    outTrace.PushBack(vec);
  }

  outTrace.SetBinning(timeSeries.GetBinning());
  outTrace.SetOffset(timeSeries.GetOffset());

  return outTrace;
}

