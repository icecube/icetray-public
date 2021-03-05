#include <I3Test.h>

#include <dataclasses/fft/FFTDataContainer.h>
#include <dataclasses/fft/FFTHilbertEnvelope.h>

TEST_GROUP(FFTDataContainer);

AntennaTimeSeries GetAntennaTimeSeries(double x) {
  AntennaTimeSeries trace;
  for (int i = 0; i < 10; i++)
    trace.PushBack(x);

  trace.SetBinning(2.);
  trace.SetOffset(0.);
  return trace;
}

EFieldTimeSeries GetEFieldTimeSeries(double x) {
  EFieldTimeSeries trace;
  for (int i = 0; i < 10; i++)
    trace.PushBack(I3Position(x, x, x));

  trace.SetBinning(2.);
  trace.SetOffset(0.);
  return trace;
}

TEST(constructor) {
  FFTDataContainer<double, std::complex<double>> fft1D;
  FFTDataContainer<I3Position, I3ComplexVector> fft3D;

  FFTDataPtr thePtr(new FFTData());

  ENSURE(1u == fft1D.GetNyquistZone());
}

TEST(copy_constructor) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(33);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);
  fftData.SetNyquistZone(37);

  const FFTData otherFFT(fftData);
  ENSURE(trace.IsSameTypeAs(otherFFT.GetConstTimeSeries()));
  ENSURE(trace == otherFFT.GetConstTimeSeries());
  ENSURE(37 == otherFFT.GetNyquistZone());
}

TEST(fourier_transform) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(1);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  AntennaSpectrum *fourier = &(fftData.GetFrequencySpectrum());
  for (unsigned int ibin = 0; ibin < fourier->GetSize(); ibin++) {
    if (0 == ibin) {
      ENSURE((*fourier)[ibin] == trace.GetSize() * trace.GetBinning());
      (*fourier)[ibin] *= 2;
    } else {
      ENSURE((*fourier)[ibin] == 0.);
      (*fourier)[ibin] = 0;
    }
  }

  AntennaTimeSeries *traceAgain = &(fftData.GetTimeSeries());
  ENSURE(*traceAgain == (trace * 2.));
}

TEST(trace_loading) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(36.5);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  AntennaTimeSeries copied = fftData.GetTimeSeries();
  ENSURE(copied == trace);

  AntennaSpectrum spectrum = fftData.GetFrequencySpectrum();
  spectrum *= 2;
  spectrum.SetBinning(147.89);
  fftData.LoadFrequencySpectrum(spectrum);
  ENSURE(spectrum == fftData.GetConstFrequencySpectrum());
}

TEST(trace_getting) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(1.6);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  AntennaTimeSeries &traceFromFft = fftData.GetTimeSeries();
  const AntennaTimeSeries &traceFromFftConst = fftData.GetConstTimeSeries();

  ENSURE(traceFromFft == traceFromFftConst);

  AntennaSpectrum spectrum = fftData.GetFrequencySpectrum();
  const AntennaSpectrum spectrumConst = fftData.GetConstFrequencySpectrum();
  ENSURE(spectrum == spectrumConst);
}

TEST(get_frequency) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(1);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  AntennaSpectrum spectrum = fftData.GetFrequencySpectrum();
  const double binning = spectrum.GetBinning();

  for (unsigned int ibin = 0; ibin < spectrum.GetSize(); ibin++) {
    const double expected = ibin * binning;
    const double value = fftData.GetFrequencyOfBin(ibin);
    ENSURE_DISTANCE(value, expected, binning * 1e-5);
  }

  //Do it again without forcing the FFT in the backend
  fftData.LoadTimeSeries(trace);
  for (unsigned int ibin = 0; ibin < spectrum.GetSize(); ibin++) {
    const double expected = ibin * binning;
    const double value = fftData.GetFrequencyOfBin(ibin);
    ENSURE_DISTANCE(value, expected, binning * 1e-5);
  }
}

TEST(get_bin_of_frequency) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(1);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  AntennaSpectrum spectrum = fftData.GetFrequencySpectrum();
  const double binning = spectrum.GetBinning();

  for (unsigned int ibin = 0; ibin < spectrum.GetSize() - 1; ibin++) {
    const double freq = ibin * binning;
    const int expected = ibin;
    const int value = fftData.GetBinOfFrequency(freq);
    ENSURE(value == expected);
  }

  //Do it again without forcing the FFT in the backend
  fftData.LoadTimeSeries(trace);
  for (unsigned int ibin = 0; ibin < spectrum.GetSize(); ibin++) {
    const double freq = ibin * binning;
    const int expected = ibin;
    const int value = fftData.GetBinOfFrequency(freq);
    ENSURE(value == expected);
  }
}

TEST(printing) {
  AntennaTimeSeries trace = GetAntennaTimeSeries(1);
  FFTData fftData;
  fftData.LoadTimeSeries(trace);

  std::cerr << fftData << std::endl;
}

TEST(container_of_3d_traces) {
  AntennaTimeSeries trace1d = GetAntennaTimeSeries(14);
  FFTData fftData1d;
  fftData1d.LoadTimeSeries(trace1d);

  AntennaSpectrum spectrum1d = fftData1d.GetFrequencySpectrum();

  EFieldTimeSeries trace3d = GetEFieldTimeSeries(14);
  FFTData3D fftData3d;
  fftData3d.LoadTimeSeries(trace3d);

  EFieldTimeSeries trace2 = fftData3d.GetTimeSeries();

  for (unsigned int ibin = 0; ibin < trace2.GetSize(); ibin++) {
    ENSURE_DISTANCE((trace2[ibin] - trace3d[ibin]).Magnitude(), 0, 1e-5);
  }

  EFieldSpectrum spectrum3d = fftData3d.GetFrequencySpectrum();

  for (unsigned int ibin = 0; ibin < spectrum3d.GetSize(); ibin++) {
    ENSURE_DISTANCE(abs(spectrum3d[ibin].GetX() - spectrum1d[ibin]), 0, 1e-5);
  }

  trace2 = fftData3d.GetTimeSeries();
  for (unsigned int ibin = 0; ibin < trace2.GetSize(); ibin++) {
    ENSURE_DISTANCE((trace2[ibin] - trace3d[ibin]).Magnitude(), 0, 1e-5);
  }
}

TEST(tests_with_warnings) {
  FFTData fftData;
  log_info("This should yell about having not set the binning then yell about empty traces");
  fftData.GetTimeSeries();
  fftData.GetFrequencySpectrum();
  fftData.GetTimeSeries();
  AntennaTimeSeries trace;
  trace.PushBack(1);
  trace.PushBack(1);
  trace.PushBack(1);
  trace.SetBinning(1);
  log_info("The following should yell about clippling the last bin");
  fftData.LoadTimeSeries(trace);
  fftData.GetFrequencySpectrum();



  FFTData3D fftData3d;
  log_info("This should yell about having not set the binning then yell about empty traces");
  fftData3d.GetTimeSeries();
  fftData3d.GetFrequencySpectrum();
  fftData3d.GetTimeSeries();
  EFieldTimeSeries trace3d;
  trace3d.PushBack(I3Position(1, 2, 3));
  trace3d.PushBack(I3Position(1, 2, 3));
  trace3d.PushBack(I3Position(1, 2, 3));
  trace3d.SetBinning(1);
  log_info("The following should yell about clippling the last bin");
  fftData3d.GetTimeSeries();
  fftData3d.GetFrequencySpectrum();
  fftData3d.GetTimeSeries();

  log_info("This should yell about setting a bad nyquist zone");
  try {
    fftData.SetNyquistZone(0);
    ENSURE(false, "You did not catch that I gave a bad nyquist zone");
  } catch (const std::exception& e) {}

  log_info("This should yell about asking for a frequency bin out of range");
  try {
    fftData.GetBinOfFrequency(1e300);
    ENSURE(false, "You did not catch that I asked for too high of a frequency");
  } catch (const std::exception& e) {}

  AntennaTimeSeries trace1d = GetAntennaTimeSeries(14);
  trace1d.SetBinning(NAN);
  log_info("This should yell about loading a time binning of NAN");
  fftData.LoadTimeSeries(trace1d);

  trace1d.SetBinning(1);
  trace1d.SetBinning(NAN);
  log_info("This should yell about loading a time offset of NAN");
  fftData.LoadTimeSeries(trace1d);

  AntennaSpectrum freqSpec;
  freqSpec.PushBack(1);
  freqSpec.PushBack(1);
  freqSpec.PushBack(1);
  freqSpec.PushBack(1);
  log_info("This should yell about loading a frequency binning of NAN");
  fftData.LoadFrequencySpectrum(freqSpec);
}
