#include <I3Test.h>
#include <icetray/I3Logging.h>

#include <dataclasses/fft/FFTWPlan.h>

TEST_GROUP(FFTWPlan);

using complexD = std::complex<double>;

void FillRealData(double *arr, unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    const double val = sin(2 * M_PI * i / (1. * n));
    arr[i] = val;
  }
}

void FillComplexData(complexD *arr, unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    arr[i] = complexD(0.5, i);
  }
}

void FillComplexData(double arr[][2], unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    arr[i][0] = 0.5;
    arr[i][1] = i;
  }
}

TEST(bin_length_conversion) {
  const unsigned int n = 50;

  ENSURE(fft::GetNCFromNR(n) == n / 2 + 1, "Length relation Real to Complex");
  ENSURE(fft::GetNRFromNC(n) == (n - 1) * 2, "Length relation Complex to Real");
}

TEST(default_constructor) {
  FFTWPlan plan;
}

TEST(initialized_constructor) {
  const unsigned int n = 10;
  FFTWPlan plan(n, fft::eR2C);
}

TEST(real_to_complex_transform) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eR2C);

  double data[n];
  FillRealData(data, n);
  plan.CopyIntoPlan(data, n);

  const uint32_t nc = fft::GetNCFromNR(n);
  complexD outData[nc];
  plan.CopyOutOfPlanC(outData);

  // if n is 4, nc will be 3
  ENSURE_EQUAL(nc, 3u);
  ENSURE_EQUAL(sizeof(outData)/sizeof(outData[0]), nc,
               "unexpected number of elemtes in outData");

  ENSURE_DISTANCE(outData[0].real(), 0, 1e-8);
  ENSURE_DISTANCE(outData[0].imag(), 0, 1e-8);
  ENSURE_DISTANCE(outData[1].real(), 0, 1e-8);
  ENSURE_DISTANCE(outData[1].imag(), -2, 1e-8);
  ENSURE_DISTANCE(outData[2].real(), 0, 1e-8);
  ENSURE_DISTANCE(outData[2].imag(), 0, 1e-8);
}

TEST(complex_to_real_transform) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eC2R);

  complexD data[n];
  FillComplexData(data, n);
  plan.CopyIntoPlanC(data, n);

  const int nr = fft::GetNRFromNC(n);
  double outData[nr];
  plan.CopyOutOfPlan(outData);
  ENSURE_DISTANCE(outData[0], 3, 1e-5);
  ENSURE_DISTANCE(outData[1], -5.19615, 1e-5);
  ENSURE_DISTANCE(outData[2], 1.73205, 1e-5);
  ENSURE_DISTANCE(outData[3], 0, 1e-5);
  ENSURE_DISTANCE(outData[4], -1.73205, 1e-5);
  ENSURE_DISTANCE(outData[5], 5.19615, 1e-5);
}

TEST(complex_to_real_transform_v2) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eC2R);

  double data[n][2];
  FillComplexData(data, n);
  plan.CopyIntoPlanC(data, n);

  const int nr = fft::GetNRFromNC(n);
  double outData[nr];
  plan.CopyOutOfPlan(outData);
  ENSURE_DISTANCE(outData[0], 3, 1e-5);
  ENSURE_DISTANCE(outData[1], -5.19615, 1e-5);
  ENSURE_DISTANCE(outData[2], 1.73205, 1e-5);
  ENSURE_DISTANCE(outData[3], 0, 1e-5);
  ENSURE_DISTANCE(outData[4], -1.73205, 1e-5);
  ENSURE_DISTANCE(outData[5], 5.19615, 1e-5);
}

TEST(complex_to_complex_transform) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eC2C);

  complexD data[n];
  FillComplexData(data, n);
  plan.CopyIntoPlanC(data, n);

  complexD outData[n];
  plan.CopyOutOfPlanC(outData);
  ENSURE_DISTANCE(abs(outData[0] - complexD(2, 6)), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[1] - complexD(-2, -2)), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[2] - complexD(0, -2)), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[3] - complexD(2, -2)), 0, 1e-5);
}

TEST(complex_to_complex_transform_v2) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eC2C);

  complexD data[n];
  FillComplexData(data, n);
  plan.CopyIntoPlanC(data, n);

  double outData[n][2];
  plan.CopyOutOfPlanC(outData);
  ENSURE_DISTANCE(abs(outData[0][0] - 2), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[1][0] - -2), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[2][0] - 0), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[3][0] - 2), 0, 1e-5);

  ENSURE_DISTANCE(abs(outData[0][1] - 6), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[1][1] - -2), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[2][1] - -2), 0, 1e-5);
  ENSURE_DISTANCE(abs(outData[3][1] - -2), 0, 1e-5);
}

TEST(printing) {
  const unsigned int n = 4;
  FFTWPlan plan(n, fft::eC2C);

  complexD data[n];
  FillComplexData(data, n);
  plan.CopyIntoPlanC(data, n);
  complexD outData[n];
  plan.CopyOutOfPlanC(outData);
  plan.PrintPlan();

  FFTWPlan plan2(n, fft::eC2R);
  plan2.CopyIntoPlanC(data, n);
  const int nr = fft::GetNRFromNC(n);
  double outData2[nr];
  plan2.CopyOutOfPlan(outData2);
  plan2.PrintPlan();
}

TEST(tests_with_warnings) {
  FFTWPlan plan(0, fft::eC2C, 0, 0);

  log_info("This will yell about you about giving the wrong amount of things");
  double data[10];
  try {
    plan.CopyIntoPlan(data, 1);
    ENSURE(0, "You didn't catch giving the wrong number of inputs");
  } catch (const std::exception& e) {
    log_info("We are good");
  }

  log_info("This will also yell about giving in the wrong about of inputs");
  complexD data2[10];
  try {
    plan.CopyIntoPlanC(data2, 2);
    ENSURE(0, "You didn't catch giving the wrong number of inputs");
  } catch (const std::exception& e) {
    log_info("We are good");
  }

  log_info("This will also yell about giving in the wrong about of inputs");
  double data3[10][2];
  try {
    plan.CopyIntoPlanC(data3, 3);
    ENSURE(0, "You didn't catch giving the wrong number of inputs");
  } catch (const std::exception& e) {
    log_info("We are good");
  }

  log_info("This will yell about not having set the plan");
  try {
    plan.CopyOutOfPlan(data, false);
    ENSURE(0, "You didn't notice that there was not a set plan");
  } catch (const std::exception& e) {
    log_info("We are good");
  }

  FFTWPlan plan2(4, fft::eR2C, 0, 0);
  plan2.CopyIntoPlan(data, 4);
  log_info("This will yell about asking for the wrong FFT type");
  try {
    plan2.CopyOutOfPlan(data, false);
    ENSURE(0, "You didn't notice that you asked for the wrong type");
  } catch (const std::exception& e) {
    log_info("We are good");
  }

  try {
    FFTWPlan plan3(10, fft::eFFTTypeCount);
    ENSURE(false, "You didn't notice when I gave an undefined FFTType enum!");
  } catch (const std::exception& e) {
    log_info("We are good");
  }
}
