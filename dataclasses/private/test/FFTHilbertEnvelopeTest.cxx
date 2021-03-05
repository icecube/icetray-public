#include <I3Test.h>
#include <dataclasses/fft/FFTHilbertEnvelope.h>

#include <cmath>

using complexD = std::complex<double>;

TEST_GROUP(FFTHilbertEnvelope);

void FillTimeSeriesWithData(AntennaTimeSeries &timeSeries, const int n, const double dt) {
  timeSeries.Clear();
  timeSeries.SetBinning(dt);
  for (int i = 0; i < n; i++) {
    const double val = sin(2 * M_PI * i / (1. * n));
    timeSeries.PushBack(val);
  }
}

void FillTimeSeriesWithData3d(EFieldTimeSeries &timeSeries, const int n, const double dt) {
  timeSeries.Clear();
  timeSeries.SetBinning(dt);
  for (int i = 0; i < n; i++) {
    const double val = sin(2 * M_PI * i / (1. * n));
    timeSeries.PushBack(I3Position(val, val, val));
  }
}

TEST(analytic_signal) {
  AntennaTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  FillTimeSeriesWithData(timeSeries, 10, 0.3);

  AntennaSpectrum analytic = fft::GetAnalyticSignal(timeSeries);
  ENSURE_DISTANCE(std::abs(analytic[0] - complexD(0, -1)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[1] - complexD(0.587785, -0.809017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[2] - complexD(0.951057, -0.309017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[3] - complexD(0.951057, 0.309017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[4] - complexD(0.587785, 0.809017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[5] - complexD(0, 1)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[6] - complexD(-0.587785, 0.809017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[7] - complexD(-0.951057, 0.309017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[8] - complexD(-0.951057, -0.309017)), 0, 1e-5);
  ENSURE_DISTANCE(std::abs(analytic[9] - complexD(-0.587785, -0.809017)), 0, 1e-5);

  ENSURE(analytic.GetBinning() == timeSeries.GetBinning());
  ENSURE(analytic.GetOffset() == timeSeries.GetOffset());
}

TEST(hilbert_envelope) {
  AntennaTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  FillTimeSeriesWithData(timeSeries, 10, 0.3);

  AntennaTimeSeries hilbert = fft::GetHilbertEnvelope(timeSeries);
  for (int i = 0; i < 10; i++) {
    ENSURE_DISTANCE(hilbert[i], 1, 1e-5);
  }
  ENSURE(hilbert.GetBinning() == timeSeries.GetBinning());
  ENSURE(hilbert.GetOffset() == timeSeries.GetOffset());
}

TEST(hilbert_peak) {
  AntennaTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  FillTimeSeriesWithData(timeSeries, 10, 0.3);

  double maxVal, peakTime;
  timeSeries[2] = 5;
  peakTime = fft::GetHilbertPeakTime(timeSeries, maxVal);
  ENSURE_DISTANCE(peakTime, (2 * 0.3) - 33, 1e-5);
  ENSURE_DISTANCE(maxVal, 5, 1e-2);

  timeSeries[8] = 13;
  peakTime = fft::GetHilbertPeakTime(timeSeries, maxVal);
  ENSURE_DISTANCE(peakTime, (8 * 0.3) - 33, 1e-5);
  ENSURE_DISTANCE(maxVal, 13, 1e-2);
}

TEST(wrappers) {
  AntennaTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  FillTimeSeriesWithData(timeSeries, 10, 0.3);

  FFTData data;
  data.LoadTimeSeries(timeSeries);

  ENSURE(fft::GetHilbertEnvelope(data) == fft::GetHilbertEnvelope(timeSeries), "FFTData wrapper works");
  ENSURE(fft::GetHilbertPeakTime(data) == fft::GetHilbertPeakTime(timeSeries), "FFTData wrapper works");
  ENSURE(fft::GetAnalyticSignal(data) == fft::GetAnalyticSignal(timeSeries), "FFTData wrapper works");

  EFieldTimeSeries timeSeries3D;
  timeSeries3D.SetOffset(-33);
  FillTimeSeriesWithData3d(timeSeries3D, 10, 0.3);
  FFTData3D fftData3d;
  fftData3d.LoadTimeSeries(timeSeries3D);
  ENSURE(fft::GetHilbertEnvelope(timeSeries3D) == fft::GetHilbertEnvelope(fftData3d), "FFTData wrapper works");
}

TEST(hilbert_peak_3d) {
  EFieldTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  FillTimeSeriesWithData3d(timeSeries, 10, 0.3);

  double maxVal, peakTime;
  timeSeries[2] = I3Position(50 / sqrt(3.), 50 / sqrt(3.), 50 / sqrt(3.));
  peakTime = fft::GetHilbertPeakTime(timeSeries, maxVal);
  ENSURE_DISTANCE(maxVal, 50, 1e-2);
  ENSURE_DISTANCE(peakTime, (2 * 0.3) - 33, 1e-5);

  timeSeries[8] = I3Position(130 / sqrt(3.), 130 / sqrt(3.), 130 / sqrt(3.));
  peakTime = fft::GetHilbertPeakTime(timeSeries, maxVal);
  ENSURE_DISTANCE(maxVal, 130, 1e-2);
  ENSURE_DISTANCE(peakTime, (8 * 0.3) - 33, 1e-5);

  FFTData3D fftData3d;
  fftData3d.LoadTimeSeries(timeSeries);
  maxVal = 0;
  peakTime = fft::GetHilbertPeakTime(fftData3d, maxVal);
  ENSURE_DISTANCE(maxVal, 130, 1e-2);
  ENSURE_DISTANCE(peakTime, (8 * 0.3) - 33, 1e-5);
}

TEST(tests_with_warnings) {
  AntennaTimeSeries timeSeries;
  timeSeries.SetOffset(-33);
  timeSeries.SetBinning(1);
  log_info("This will yell about having an empty trace");
  fft::GetHilbertEnvelope(timeSeries);

  timeSeries.SetOffset(NAN);
  log_info("This will yell about getting peak time with unitialized binning");
  double dump;
  fft::GetHilbertPeakTime(timeSeries, dump);

  log_info("This will yell about getting peak time with unitialized binning");
  EFieldTimeSeries timeSeries3D;
  timeSeries.SetBinning(1);
  fft::GetHilbertPeakTime(timeSeries3D, dump);
}
