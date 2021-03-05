#include <I3Test.h>

#include <dataclasses/I3AntennaChannel.h>

TEST_GROUP(I3AntennaChannel);

FFTData GetFFTDataContainer() {
  FFTData fftData;

  AntennaTimeSeries trace;
  trace.SetBinning(2.);
  trace.SetOffset(10);
  for (int i = 0; i < 10; i++) {
    trace.PushBack(5 - abs(i - 5));
  }

  fftData.LoadTimeSeries(trace);

  return fftData;
}

TEST(default_constructor) {
  I3AntennaChannel chData;
}

TEST(initialized_constructor) {
  I3AntennaChannel chData1;
  chData1.GetFFTData() = GetFFTDataContainer();

  FFTData fftData = GetFFTDataContainer();
  I3AntennaChannel chData2(fftData);

  ENSURE(chData1.GetMaxAmplitude() == chData2.GetMaxAmplitude());
  ENSURE(chData1.GetTimeAtMaximum() == chData2.GetTimeAtMaximum());
}

TEST(initialized_constructor2) {
  FFTData fftData = GetFFTDataContainer();
  I3AntennaChannel chData1(fftData);

  I3AntennaChannel chData2(chData1);

  ENSURE(chData1.GetMaxAmplitude() == chData2.GetMaxAmplitude());
  ENSURE(chData1.GetTimeAtMaximum() == chData2.GetTimeAtMaximum());
}

TEST(get_ffts) {
  FFTData fftData = GetFFTDataContainer();
  I3AntennaChannel chData(fftData);
  FFTData fftData1 = chData.GetFFTData();
  const FFTData fftData2 = chData.GetConstFFTData();

  AntennaSpectrum spectrum1 = fftData1.GetFrequencySpectrum();
  const AntennaSpectrum spectrum2 = fftData2.GetConstFrequencySpectrum();

  ENSURE(spectrum1 == spectrum2);
}

TEST(printing) {
  FFTData fftData = GetFFTDataContainer();
  I3AntennaChannel chData(fftData);

  std::cerr << chData << std::endl;
}