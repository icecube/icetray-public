#include <I3Test.h>

#include <dataclasses/fft/FFTResamplingTools.h>

using std::sin;

TEST_GROUP(ResamplingTools);

void FillData(FFTData &fftData, int n, double dt, double t0) {
  AntennaTimeSeries *trace = &(fftData.GetTimeSeries());
  trace->Clear();
  trace->SetBinning(dt);
  trace->SetOffset(t0);
  for (int ibin = 0; ibin < n; ibin++) {
    double t = t0 + dt * ibin;
    double amp = 4 * sin(t * 2. / (2.* M_PI) + M_PI / 4.)
                 - 3 * sin(t * 5. / (2.* M_PI) + M_PI / 9.)
                 + 2.1 * sin(t * 7. / (2.* M_PI) + M_PI / 3.);
    trace->PushBack(amp);
  }
}

void FillData2(FFTData &fftData, int n, double dt, double t0) {
  AntennaTimeSeries *trace = &(fftData.GetTimeSeries());
  trace->Clear();
  trace->SetBinning(dt);
  trace->SetOffset(t0);
  for (int ibin = 0; ibin < n; ibin++) {
    double t = t0 + dt * ibin;
    double amp = t * t * 3 + 5;
    trace->PushBack(amp);
  }
}

TEST(upsample_time_series) {
  FFTData data1;
  FillData(data1, 20, 1.5, -36);

  FFTData data2;
  FillData(data2, 20, 1.5, -36);

  fft::UpsampleTimeSeries(data2, 1, false);

  AntennaTimeSeries &trace1 = data1.GetTimeSeries();
  AntennaTimeSeries &trace2 = data2.GetTimeSeries();
  ENSURE_DISTANCE(trace2.GetBinning(), trace1.GetBinning(), 1e-8, "Factor of 1 does nothing");
  ENSURE_DISTANCE(trace2.GetSize(), trace1.GetSize(), 1e-8, "Factor of 1 does nothing");
  for (unsigned int ibin = 0; ibin < trace1.GetSize(); ibin++) {
    ENSURE_DISTANCE(trace2[ibin], trace1[ibin], 1e-8, "Factor of 1 does nothing");
  }

  FillData(data1, 10, 1.5, -36);
  FillData(data2, 10, 1.5, -36);
  fft::UpsampleTimeSeries(data2, 2, false);
  trace1 = data1.GetTimeSeries();
  trace2 = data2.GetTimeSeries();
  ENSURE_DISTANCE(trace2.GetSize(), trace1.GetSize() * 2, 1e-8, "Factor of 1 does nothing");
  ENSURE_DISTANCE(trace2[0], 1.63686, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[1], 3.99363, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[2], 6.22206, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[3], 6.89187, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[4], 5.16137, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[5], 1.42796, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[6], -2.93547, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[7], -6.22825, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[8], -7.20319, 1e-5, "Upsample test");
  ENSURE_DISTANCE(trace2[9], -5.80329, 1e-5, "Upsample test");

  FillData(data2, 10, 1.5, -36);
  fft::UpsampleTimeSeries(data2, 2, true);
  AntennaSpectrum &freq2 = data2.GetFrequencySpectrum();
  ENSURE_DISTANCE(abs(freq2[0]), 0, 1e-5);
}

TEST(thin_out_time_series) {
  FFTData data1;
  FillData2(data1, 20, 1.5, -36);

  FFTData data2;
  FillData2(data2, 20, 1.5, -36);

  fft::ThinOutTimeSeries(data2, 1);

  AntennaTimeSeries &trace1 = data1.GetTimeSeries();
  AntennaTimeSeries &trace2 = data2.GetTimeSeries();
  ENSURE_DISTANCE(trace2.GetBinning(), trace1.GetBinning(), 1e-8, "Factor of 1 does nothing");
  ENSURE_DISTANCE(trace2.GetSize(), trace1.GetSize(), 1e-8, "Factor of 1 does nothing");
  for (unsigned int ibin = 0; ibin < trace1.GetSize(); ibin++) {
    ENSURE_DISTANCE(trace2[ibin], trace1[ibin], 1e-8, "Factor of 1 does nothing");
  }

  FillData2(data1, 50, 1.5, -36);
  FillData2(data2, 50, 1.5, -36);
  fft::ThinOutTimeSeries(data2, 5);

  trace1 = data1.GetTimeSeries();
  trace2 = data2.GetTimeSeries();
  ENSURE_DISTANCE(trace2.GetSize() * 5, trace1.GetSize(), 1e-8, "Factor of 1 does nothing");
  for (unsigned int ibin = 0; ibin < trace2.GetSize(); ibin++) {
    double t = -36 + trace2.GetBinning() * ibin;
    double amp = t * t * 3 + 5;
    ENSURE_DISTANCE(trace2[ibin], amp, amp * 1e-5, "Downsample test");
  }
}

TEST(resample) {
  FFTData data;
  FillData(data, 20, 1.0, -36);

  fft::ResampleFFTDataContainer(data, 2.5);

  AntennaTimeSeries timeseries = data.GetTimeSeries();
  ENSURE(timeseries.GetSize() == 8);
  ENSURE(timeseries.GetBinning() == 2.5);
  ENSURE(timeseries.GetOffset() == -36);
  ENSURE_DISTANCE(timeseries[0],  4.02425, 1e-5, "Downsample test");
  ENSURE_DISTANCE(timeseries[1],  6.13505, 1e-5, "Downsample test");
  ENSURE_DISTANCE(timeseries[2], -5.33249, 1e-5, "Downsample test");
  ENSURE_DISTANCE(timeseries[3], -3.05589, 1e-5, "Downsample test");
  ENSURE_DISTANCE(timeseries[4], -1.84382, 1e-5, "Downsample test");
  ENSURE_DISTANCE(timeseries[5], -2.54592, 1e-5, "Downsample test");
}