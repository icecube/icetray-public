#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/complex.hpp>

#include <dataclasses/fft/FFTDataContainer.h>
#include <dataclasses/fft/FFTWPlan.h>


/////////////////////////////////////////
////////Time:double, Freq:complex////////
/////////////////////////////////////////

template<>
void FFTDataContainer<double, std::complex<double>>::UpdateTimeSeries() {
  if (std::isnan(frequencySpectrum_.GetBinning())) {
    log_warn("The binning on the frequency spectrum has not been set yet. Things are about to go south.");
  }

  const unsigned int nComp = frequencySpectrum_.GetSize();
  const unsigned int nReal = fft::GetNRFromNC(nComp);

  if (nComp == 0) {
    upToDateDomain_ = Both;
    log_warn("Trying to do FFT on empty frequency spectrum! No Can do, Boss!");
    return;
  }

  double realData[nReal];
  std::complex<double> complexData[nComp];

  for (unsigned int i = 0; i < nComp; i++) {
    complexData[i] = frequencySpectrum_[i] * frequencySpectrum_.GetBinning();
  }

  FFTWPlan plan(nComp, fft::eC2R); //Make FFTW plan
  plan.CopyIntoPlanC(complexData, nComp);
  plan.ExecutePlan();  //Actually do the FFT
  plan.CopyOutOfPlan(realData);  //Copy the result out of the fft

  timeSeries_.CopyIntoTrace(realData, nReal);
  timeSeries_.SetBinning(1. / frequencySpectrum_.GetBinning() / nReal);

  upToDateDomain_ = Both;
}

template<>
void FFTDataContainer<double, std::complex<double>>::UpdateFrequencySpectrum() {
  if (std::isnan(timeSeries_.GetBinning())) {
    log_error("The binning on the time series has not been set yet. Failing FFT.");
  }

  const unsigned int testReal = timeSeries_.GetSize();
  if ((testReal / 2) * 2 != testReal) {
    timeSeries_.PopBack();
    log_warn("Trying to perform FFT on a time series with uneven number of bins -- clipped by one before FFT");
  }

  const unsigned int nReal = timeSeries_.GetSize();
  const unsigned int nComp = fft::GetNCFromNR(nReal);

  if (0 == nReal) {
    upToDateDomain_ = Both;
    log_warn("trying to do FFT on empty time series! No can do, Boss!");
    return;
  }

  double realData[nReal];
  std::complex<double> complexData[nComp];

  for (unsigned int i = 0; i < nReal; i++) {
    realData[i] = timeSeries_[i] * timeSeries_.GetBinning();
  }

  FFTWPlan plan(nReal, fft::eR2C); //Make FFTW plan
  plan.CopyIntoPlan(realData, nReal);
  plan.ExecutePlan();  //Actually do the FFT
  plan.CopyOutOfPlanC(complexData);  //Copy the result out of the fft

  frequencySpectrum_.CopyIntoTrace(complexData, nComp);
  frequencySpectrum_.SetBinning(0.5 / timeSeries_.GetBinning() / (double(nComp) - 1));

  upToDateDomain_ = Both;
}


/////////////////////////////////////////////////
//Time:AntennaTimeSeries, Freq:AntennaSpectrum///
/////////////////////////////////////////////////

template<>
void
FFTDataContainer<I3Position, I3ComplexVector>::UpdateTimeSeries() {
  if (std::isnan(frequencySpectrum_.GetBinning())) {
    log_warn("The binning on the time series has not been set yet. Things are about to go south.");
  }

  const unsigned int nComp = frequencySpectrum_.GetSize();
  const unsigned int nReal = fft::GetNRFromNC(nComp);

  if (nComp == 0) {
    upToDateDomain_ = Both;
    log_error("The binning on the frequency spectrum has not been set yet. Failing FFT.");
    return;
  }

  double realData[3][nReal];
  std::complex<double> complexData[3][nComp];
  I3Position resultData[nReal];

  //Convert I3Position to array for fftw
  for (unsigned int i = 0; i < nComp; i++) {
    complexData[0][i] = frequencySpectrum_[i].GetX() * frequencySpectrum_.GetBinning();
    complexData[1][i] = frequencySpectrum_[i].GetY() * frequencySpectrum_.GetBinning();
    complexData[2][i] = frequencySpectrum_[i].GetZ() * frequencySpectrum_.GetBinning();
  }

  FFTWPlan plan(nComp, fft::eC2R); //Make FFTW plan

  //Do the FFT for each dimension
  for (unsigned int i = 0; i < 3; i++) {
    plan.CopyIntoPlanC(&complexData[i][0], nComp);
    plan.ExecutePlan();  //Actually do the FFT
    plan.CopyOutOfPlan(&realData[i][0]);  //Copy the result out of the fft
  }

  //Convert from array to I3Position
  for (unsigned int i = 0; i < nReal; i++) {
    resultData[i].SetX(realData[0][i]);
    resultData[i].SetY(realData[1][i]);
    resultData[i].SetZ(realData[2][i]);
  }

  timeSeries_.CopyIntoTrace(resultData, nReal);
  timeSeries_.SetBinning(1. / frequencySpectrum_.GetBinning() / nReal);

  upToDateDomain_ = Both;
}



template<>
void FFTDataContainer<I3Position, I3ComplexVector>::UpdateFrequencySpectrum() {
  if (std::isnan(timeSeries_.GetBinning())) {
    log_error("The binning on the time series has not been set yet. Failing FFT.");
  }

  const unsigned int testReal = timeSeries_.GetSize();
  if ((testReal / 2) * 2 != testReal) {
    timeSeries_.PopBack();
    log_warn("Trying to perform FFT on a time series with uneven number of bins -- clipped by one before FFT");
  }

  const unsigned int nReal = timeSeries_.GetSize();
  const unsigned int nComp = fft::GetNCFromNR(nReal);

  if (0 == nReal) {
    upToDateDomain_ = Both;
    log_warn("Trying to do FFT on empty time series! No Can do, Boss!");
    return;
  }

  double realData[3][nReal];
  std::complex<double> complexData[3][nComp];
  I3ComplexVector resultData[nComp];

  //Convert vector to array for fftw
  for (unsigned int i = 0; i < nReal; i++) {
    realData[0][i] = timeSeries_[i].GetX() * timeSeries_.GetBinning();
    realData[1][i] = timeSeries_[i].GetY() * timeSeries_.GetBinning();
    realData[2][i] = timeSeries_[i].GetZ() * timeSeries_.GetBinning();
  }

  FFTWPlan plan(nReal, fft::eR2C); //Make FFTW plan

  //Do the FFT for each dimension
  for (unsigned int i = 0; i < 3; i++) {
    plan.CopyIntoPlan(&realData[i][0], nReal);
    plan.ExecutePlan();  //Actually do the FFT
    plan.CopyOutOfPlanC(&complexData[i][0]);  //Copy the result out of the fft
  }

  //Convert from array to vector
  for (unsigned int i = 0; i < nReal; i++) {
    resultData[i].SetX(complexData[0][i]);
    resultData[i].SetY(complexData[1][i]);
    resultData[i].SetZ(complexData[2][i]);
  }

  frequencySpectrum_.CopyIntoTrace(resultData, nComp);
  frequencySpectrum_.SetBinning(0.5 / timeSeries_.GetBinning() / (double(nComp) - 1));

  upToDateDomain_ = Both;
}

template<typename T, typename F>
template <class Archive>
void FFTDataContainer<T, F>::serialize(Archive& ar, unsigned version) {
  if (version > i3fftdatacontainer_version_)
    log_fatal("Attempting to read version %u from file but running version %u of FFTDataContainer class.", version, i3fftdatacontainer_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("TimeSeries", timeSeries_);
  ar & make_nvp("FrequencySpectrum", frequencySpectrum_);
  ar & make_nvp("NyquistZone", nyquistZone_);
  ar & make_nvp("UpdateStatus", upToDateDomain_);
}

I3_SERIALIZABLE(FFTData);
I3_SERIALIZABLE(FFTData3D);

I3_SERIALIZABLE(FFTDataMap);
I3_SERIALIZABLE(FFTData3DMap);
