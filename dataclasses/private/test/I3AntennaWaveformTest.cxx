#include <I3Test.h>

#include <dataclasses/I3AntennaWaveform.h>

using std::complex;
using complexD = complex<double>;

TEST_GROUP(I3AntennaWaveform);

//First helper functions to fill the traces in different ways
double FillFunction(int i, int method) {
  if (0 == method) {
    return 0;
  } else if (1 == method) {
    return i - i * i + 1;
  } else if (2 == method) {
    return i + 1;
  }
  ENSURE(false, "Really? you messed up writing the test?");
  return 0;
}
void FillWithJunk(I3AntennaWaveform<double> &trace, int n, int method) {
  trace.SetBinning(1);
  trace.SetOffset(-1);
  for (int i = 0; i < n; i++) {
    double val = FillFunction(i, method);
    trace.PushBack(val);
  }
}
//////////////////////////////////////////////////
template<typename T>
void constructor_test() {
  const I3AntennaWaveform<T> trace;
  ENSURE(std::isnan(trace.GetBinning()));
  ENSURE(std::isnan(trace.GetOffset()));
}
TEST(constructor) {
  constructor_test<double>();
  constructor_test<complexD>();
  constructor_test<I3Position>();
  constructor_test<I3ComplexVector>();
}
//////////////////////////////////////////////////

template<typename T>
void pointers_test() {
  boost::shared_ptr<I3AntennaWaveform<T>> trace(new I3AntennaWaveform<T>());
}
TEST(pointers) {
  pointers_test<double>();
  pointers_test<complexD>();
  pointers_test<I3Position>();
  pointers_test<I3ComplexVector>();
}
//////////////////////////////////////////////////

TEST(copy_constructor) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 57, 1);
  trace.SetBinning(23.);
  trace.SetOffset(-21.);
  ENSURE(57 == trace.GetSize());
  const I3AntennaWaveform<double> testTrace(trace);
  ENSURE(testTrace.GetSize() == trace.GetSize());
  ENSURE(testTrace.GetBinning() == trace.GetBinning());
  ENSURE(testTrace.GetOffset() == trace.GetOffset());
  ENSURE(testTrace.IsSameTypeAs(trace));
  for (int i = 0; i < 57; i++) {
    ENSURE(testTrace[i] == trace[i]);
  }
}
//////////////////////////////////////////////////


TEST(indexing) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 57, 1);

  const I3AntennaWaveform <double> newTrace(trace);
  for (int i = 0; i < 57; i++) {
    ENSURE(trace[i] == newTrace[i]);
    ENSURE(trace[i] == trace[i]);
    ENSURE(trace[i] == FillFunction(i, 1));
  }

  for (int i = 0; i < 57; i++) {
    trace[i] = FillFunction(i, 2);
  }

  for (int i = 0; i < 57; i++) {
    ENSURE(trace[i] == FillFunction(i, 2));
  }
}
//////////////////////////////////////////////////

TEST(iterator) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 23, 2);
  const I3AntennaWaveform<double> traceConst(trace);

  std::vector<double>::iterator begin = trace.Begin();
  std::vector<double>::iterator end = trace.End();

  std::vector<double>::const_iterator beginConst = traceConst.Begin();
  std::vector<double>::const_iterator endConst = traceConst.End();

  ENSURE(end - begin == 23);
  ENSURE(endConst - beginConst == 23);
}
//////////////////////////////////////////////////

TEST(comparison) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 5, 0);

  I3AntennaWaveform<double> trace2 = trace;
  ENSURE(trace == trace2);

  trace2[1] = -11;
  ENSURE(trace != trace2);
}
//////////////////////////////////////////////////

TEST(setters_n_getters) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 57, 2);

  trace.SetBinning(13);
  ENSURE(trace.GetBinning() == 13);
  trace.SetBinning(-57.12);
  ENSURE(trace.GetBinning() == -57.12);

  trace.SetOffset(-99);
  ENSURE(trace.GetOffset() == -99);
  trace.SetOffset(157.3);
  ENSURE(trace.GetOffset() == 157.3);

  trace.SetBin(15, -33);
  trace.SetBin(16, -34);
  trace.SetBin(17, -35);
  ENSURE(trace.GetBin(15) == -33);
  ENSURE(trace.GetBin(16) == -34);
  ENSURE(trace.GetBin(17) == -35);
}
//////////////////////////////////////////////////

TEST(data_augmentation) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 57, 2);

  const int initialSize = trace.GetSize();
  ENSURE(57 == initialSize);

  trace.PopBack();
  ENSURE((initialSize - 1) == (int) trace.GetSize());

  trace.Clear();
  ENSURE(0 == trace.GetSize());

  ENSURE(trace.Begin() == trace.End());
}
//////////////////////////////////////////////////


TEST(out_of_place_operators) {
  const int n = 57;
  const double val = 13.41;
  I3AntennaWaveform<double> trace1;
  FillWithJunk(trace1, n, 1);
  trace1.SetBinning(13.);
  trace1.SetOffset(-69.);
  I3AntennaWaveform<double> trace2;
  FillWithJunk(trace2, n, 2);
  trace2.SetBinning(trace1.GetBinning());
  trace2.SetOffset(trace1.GetOffset());

  I3AntennaWaveform<double> testAdd = trace2 + trace1;
  ENSURE(n == (int) testAdd.GetSize());
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testAdd[i], FillFunction(i, 2) + FillFunction(i, 1), 1e-6);
  }

  I3AntennaWaveform<double> testSubstract = trace2 - trace1;
  ENSURE(n == (int) testSubstract.GetSize());
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testSubstract[i], FillFunction(i, 2) - FillFunction(i, 1), 1e-6);
  }

  I3AntennaWaveform<double> testMultiply = trace2 * val;
  ENSURE(n == (int) testMultiply.GetSize());
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testMultiply[i], FillFunction(i, 2) * val, 1e-6);
  }

  I3AntennaWaveform<double> testDivide = trace2 / val;
  ENSURE(n == (int) testDivide.GetSize());
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testDivide[i], FillFunction(i, 2) / val, 1e-6);
  }
}
//////////////////////////////////////////////////

TEST(in_place_operators) {
  const int n = 57;
  const double val = 13.41;
  I3AntennaWaveform<double> baseTrace;
  FillWithJunk(baseTrace, n, 1);
  baseTrace.SetBinning(13.);
  baseTrace.SetOffset(-69.);

  I3AntennaWaveform<double> augmentor;
  FillWithJunk(augmentor, n, 2);
  augmentor.SetBinning(baseTrace.GetBinning());
  augmentor.SetOffset(baseTrace.GetOffset());


  I3AntennaWaveform<double> testTrace;

  testTrace = baseTrace;
  testTrace += augmentor;
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testTrace[i], FillFunction(i, 1) + FillFunction(i, 2), 1e-6);
  }

  testTrace = baseTrace;
  testTrace -= augmentor;
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testTrace[i], FillFunction(i, 1) - FillFunction(i, 2), 1e-6);
  }

  testTrace = baseTrace;
  testTrace *= val;
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testTrace[i], FillFunction(i, 1) * val, 1e-6);
  }

  testTrace = baseTrace;
  testTrace /= val;
  for (int i = 0; i < n; i++) {
    ENSURE_DISTANCE(testTrace[i], FillFunction(i, 1) / val, 1e-6);
  }
}
//////////////////////////////////////////////////

TEST(swap) {
  const int n = 57;
  I3AntennaWaveform<double> trace1;
  FillWithJunk(trace1, n, 2);
  trace1.SetBinning(5.5);

  I3AntennaWaveform<double> trace2;
  FillWithJunk(trace2, n + 45, 1);
  trace2.SetBinning(99.99);

  trace1.Swap(trace2);


  ENSURE(trace1.GetBinning() == 99.99);
  ENSURE(trace1.GetSize() == (n + 45));

  for (int i = 0; i < n; i++) {
    ENSURE(trace1[i] == (i - i * i + 1));
  }
}
//////////////////////////////////////////////////

TEST(printing) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 5, 0);

  std::cerr << trace << std::endl;
}
//////////////////////////////////////////////////

TEST(subset) {
  I3AntennaWaveform<double> trace;
  FillWithJunk(trace, 100, 0);

  I3AntennaWaveform<double> firstHalf = trace.GetSubset(0, 49);
  I3AntennaWaveform<double> secondHalf = trace.GetSubset(50, 99);

  ENSURE(trace.GetBinning() == firstHalf.GetBinning());
  ENSURE(trace.GetBinning() == secondHalf.GetBinning());

  ENSURE(trace.GetOffset() == firstHalf.GetOffset());
  ENSURE(trace.GetOffset() != secondHalf.GetOffset() + 50 * secondHalf.GetBinning());

  for (int ibin = 0; ibin < (int) firstHalf.GetSize(); ibin++) {
    ENSURE(trace[ibin] == firstHalf[ibin]);
    ENSURE(trace[ibin+50] == firstHalf[ibin]);
  }
}