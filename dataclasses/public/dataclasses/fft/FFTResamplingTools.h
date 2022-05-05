////////////////////////////////////////////////////////////
//Author: Alan Coleman
//Tools to resample waveforms to a different dt binning
////////////////////////////////////////////////////////////
//FFTDatacontainers can be resampled with this class to have them be output
//with a new binning without distorting the waveform. This is done in two steps
//The original and target binning are converted to a rational number to do this
//in two steps. First the times series is upsampled by zero-padding the frequency
//domain. Then the time series is thinned out by simply dropping all but every N bins.
//Generally one would only need to call ResampleFFTDataContainer.

#ifndef FFTRESAMPLETOOLS_INCLUDE_H
#define FFTRESAMPLETOOLS_INCLUDE_H

#include <dataclasses/fft/FFTDataContainer.h>
#include <boost/rational.hpp>

namespace fft {

boost::rational<long> GetRationalNumberFromDecimal(double decimal, const bool secondCall = false);

//////////////////////////////////////

template <typename T, typename F>
void UpsampleTimeSeries(FFTDataContainer<T, F> &container, const unsigned int upsamplingFactor, const bool removeOffset = false) {

  I3AntennaWaveform<F>& spectrum = container.GetFrequencySpectrum();

  const F offset = spectrum[0]; //Save this for later
  spectrum[0] *= 0;

  //Init a new spectrum with all zeros
  F zero = spectrum[0] * 0.; //Set using existing value because default for I3ComplexVector is NAN
  I3AntennaWaveform<F> upsampledSpectrum((spectrum.GetSize() - 1)*upsamplingFactor + 1, spectrum.GetBinning(), zero);
  upsampledSpectrum.SetOffset(spectrum.GetOffset());

  // as the values must be filled in in the proper order there are two cases
  // as the order of the frequencies depends on the Nyquist zone
  if (container.GetNyquistZone() == 1) {
    for (unsigned int i = 0; i < spectrum.GetSize(); ++i)
      upsampledSpectrum[i] = spectrum[i];
  } else {
    log_error("Upsampling for Nyquist zones greater than 1 not implemented!");
  }

  // replace the original spectrum by the new one (after zero padding)
  spectrum.Swap(upsampledSpectrum);

  // Add old mean to retain offset, if substraction is not requested
  if (!removeOffset)
    spectrum[0] = offset;

  // the new spectrum is always in the 1st Nyquist zone
  container.SetNyquistZone(1);
}

template <typename T, typename F>
void TruncateFrequencySpectrum(FFTDataContainer<T, F> &container, const double targetBinning) {

  I3AntennaWaveform<F>& spectrum = container.GetFrequencySpectrum();
  const double freqBinning = spectrum.GetBinning();

  const double nyquistFreq = 0.5 / targetBinning;

  //Remove all frequencies above the Nyquist frequency
  for (unsigned int ibin = 0; ibin < spectrum.GetSize(); ibin++) {
    if (ibin * freqBinning > nyquistFreq)
      spectrum[ibin] *= 0;
  }
}

template <typename T, typename F>
void ThinOutTimeSeries(FFTDataContainer<T, F> &container, const long thinOutFactor) {
  // Get the time series
  I3AntennaWaveform<T>& timeseries = container.GetTimeSeries();
  I3AntennaWaveform<T> thinnedtimeseries(static_cast<unsigned int>(timeseries.GetSize() / thinOutFactor), timeseries.GetBinning()*thinOutFactor);
  thinnedtimeseries.SetOffset(timeseries.GetOffset());

  // fill the thinned out time series
  for (unsigned int i = 0; i < thinnedtimeseries.GetSize(); ++i)
    thinnedtimeseries[i] = timeseries[i * thinOutFactor];

  // overwrite original time series
  timeseries.Swap(thinnedtimeseries);
}


template <typename T, typename F>
void ResampleFFTDataContainer(FFTDataContainer<T, F> &container, const double timeBinning) {
  if (timeBinning <= 0) {
    log_error("Asking for resampled binning of %f. Anything <=0 is meaningless. Try again.", timeBinning);
  }

  double origBinning = container.GetConstTimeSeries().GetBinning();
  double targetBinning = timeBinning;

  const boost::rational<long> origBinningRational = fft::GetRationalNumberFromDecimal(origBinning);
  const boost::rational<long> targetBinningRational = fft::GetRationalNumberFromDecimal(targetBinning);
  const boost::rational<long> resamplingFactor = origBinningRational / targetBinningRational;

  if (resamplingFactor.numerator() != 1) {
    UpsampleTimeSeries(container, resamplingFactor.numerator(), false);
  }
  if (resamplingFactor.denominator() != 1) {
    TruncateFrequencySpectrum(container, targetBinning);
    ThinOutTimeSeries(container, resamplingFactor.denominator());
  }

  container.GetTimeSeries().SetBinning(timeBinning);
}

}//fft

#endif
