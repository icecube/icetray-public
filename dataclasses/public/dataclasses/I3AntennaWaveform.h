////////////////////////////////////////////////////////////
//Implemented by: Alan Coleman
//Based heavily on the Pierre Auger Collaboration's Trace.h class
//Generic template class for time/freq traces
////////////////////////////////////////////////////////////


#ifndef I3ANTENNAWAVEFORM_INCLUDE_H
#define I3ANTENNAWAVEFORM_INCLUDE_H

#include <functional>
#include <iostream>
#include <vector>
#include <complex>

#include <dataclasses/AntennaKey.h>
#include <dataclasses/Utility.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3Vector.h>

#include <dataclasses/I3Position.h>
#include <dataclasses/I3ComplexVector.h>


static const unsigned int i3antennawaveform_version_ = 0;

template<typename T>
class I3AntennaWaveform : public I3FrameObject {
 public:
  typedef typename std::vector<T>::size_type SizeType;
  typedef typename std::vector<T>::iterator Iterator;
  typedef typename std::vector<T>::const_iterator ConstIterator;

  I3AntennaWaveform() : binning_(NAN), offset_(NAN) {}
  ~I3AntennaWaveform() {}

  I3AntennaWaveform(const SizeType size, const double binning, const T& init = T())
    : dataArray_(size, init), binning_(binning), offset_(NAN) {}

  Iterator Begin() { return dataArray_.begin(); }
  Iterator End() { return dataArray_.end(); }

  ConstIterator Begin() const { return dataArray_.begin(); }
  ConstIterator End() const { return dataArray_.end(); }

  ////////////////////////////////
  ////////Setters/Getters/////////
  ////////////////////////////////

  double GetBinning() const {
    return binning_;
  }
  void SetBinning(const double binning) {
    binning_ = binning;
  }

  SizeType GetSize() const {return dataArray_.size();}

  double GetOffset() const {
    if (NAN == offset_)
      log_fatal("Asking for binning which is NAN.");
    return offset_;
  }

  void SetOffset(double offset) {
    offset_ = offset;
  }

  ////////////////////////////////
  ////////Array Manipulate////////
  ////////////////////////////////

  //Copies passed in data into the trace. Overwrites the size
  void CopyIntoTrace(const T* toCopy, const SizeType size) {
    dataArray_.resize(size);
    copy(toCopy, toCopy + size, dataArray_.begin());
  }

  //Insert a single value at the end of the trace
  void PushBack(const T& value) {
    dataArray_.push_back(value);
  }

  //Remove one value at the end of the trace
  void PopBack() {
    dataArray_.pop_back();
  }

  void Swap(I3AntennaWaveform<T>& t) {
    std::swap(dataArray_, t.dataArray_);
    std::swap(binning_, t.binning_);
  }

  I3AntennaWaveform<T> GetSubset(unsigned int minBin, unsigned int maxBin) const {
    if (maxBin >= GetSize())
      log_fatal("Requesting a subset of bins (%d, %d) out of range (0, %d)", minBin, maxBin, (int) GetSize() - 1);

    if (minBin > maxBin)
      log_warn("Requesting subset where minBin > maxBin (%d > %d) ", minBin, maxBin);

    I3AntennaWaveform<T> trace;
    trace.binning_ = binning_;
    trace.offset_ = offset_ + minBin * binning_;

    for (unsigned int ibin = minBin; ibin <= maxBin; ibin++)
      trace.PushBack(dataArray_[ibin]);

    return trace;
  }

  //This function is mostly used for purging data before storage
  //Do not reset the time offset/binning
  void Clear() {dataArray_.clear();}

  bool IsSameTypeAs(const I3AntennaWaveform<T>& t) const {
    return dataArray_.size() == t.dataArray_.size() && binning_ == t.binning_;
  }

  bool operator==(const I3AntennaWaveform<T>& t) const
  { return IsSameTypeAs(t) && dataArray_ == t.dataArray_; }

  bool operator!=(const I3AntennaWaveform<T>& trace) const
  { return !operator==(trace); }

  ////////////////////////////////
  ////////////Operators///////////
  ////////////////////////////////

  I3AntennaWaveform<T> operator+(const I3AntennaWaveform<T>& trace) const;
  I3AntennaWaveform<T> operator-(const I3AntennaWaveform<T>& trace) const;

  I3AntennaWaveform<T>& operator+=(const I3AntennaWaveform<T>& trace);
  I3AntennaWaveform<T>& operator-=(const I3AntennaWaveform<T>& trace);

  I3AntennaWaveform<T>& operator+=(const T value);
  I3AntennaWaveform<T>& operator-=(const T value);

  I3AntennaWaveform<T>& operator*=(const double value);
  I3AntennaWaveform<T>& operator/=(const double value);
  I3AntennaWaveform<T> operator*(const double value) const;
  I3AntennaWaveform<T> operator/(const double value) const;

  T& operator[](const int i) {
    if (i >= (int) dataArray_.size() || i < -1. * dataArray_.size())
      log_fatal("Requesting index %d out of range %d", i, (int) dataArray_.size());

    if (i < 0) //Allow python-like indexing with negative numbers
      return dataArray_[dataArray_.size() + i];

    return dataArray_[i];
  }

  const T& operator[](const int i) const {
    if (i >= (int) dataArray_.size() || i < -1. * dataArray_.size())
      log_fatal("Requesting index %d out of range %d", i, (int) dataArray_.size());

    if (i < 0) //Allow python-like indexing with negative numbers
      return dataArray_[dataArray_.size() + i];

    return dataArray_[i];
  }

  //These exist to allow easy indexing for python
  T GetBin(const int i) const {return (*this)[i];}
  void SetBin(const int i, T val) {(*this)[i] = val;}

  friend std::ostream &operator<<(std::ostream &os, const I3AntennaWaveform <T> &rhs) {
    os << "I3AntennaWaveform( ";
    for (unsigned int i = 0; i < rhs.dataArray_.size(); i++) {
      os << rhs.dataArray_[i] << ' ';
    }
    os << " )";
    return os;
  }

  std::ostream& Print(std::ostream& oss) const override {
    oss << *this;
    return oss;
  }

  I3AntennaWaveform(const I3AntennaWaveform<T>& wvf):
    dataArray_(wvf.dataArray_),
    binning_(wvf.binning_),
    offset_(wvf.offset_) {
  }

  //Rotate the elements in the array by the specified amount
  //Works like numpy.roll command
  void Roll(int i) {
    if (i < 0) {
      std::rotate(dataArray_.begin(), dataArray_.begin() - i, dataArray_.end());
    } else if (i > 0) {
      std::rotate(dataArray_.begin(), dataArray_.begin() + dataArray_.size() - i, dataArray_.end());
    }
  }

 private:
  std::vector<T> dataArray_;

  double binning_;
  double offset_;

  friend class icecube::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, unsigned int version);

};


/////////////////////////////////////////////////////
//////////////Ease of use definitions////////////////
/////////////////////////////////////////////////////

//1D types
typedef I3AntennaWaveform<double> AntennaTimeSeries;
typedef I3AntennaWaveform<std::complex <double>> AntennaSpectrum;
//3D types
typedef I3AntennaWaveform<I3Position> EFieldTimeSeries;
typedef I3AntennaWaveform<I3ComplexVector> EFieldSpectrum;


/////////////////////////////////////////////////////
////////////////////IceTray Defs/////////////////////
/////////////////////////////////////////////////////

#define DO_DEFINE_IC(type)                            \
  I3_CLASS_VERSION(type, i3antennawaveform_version_); \
  I3_POINTER_TYPEDEFS(type);                          \
  typedef I3Map<AntennaKey, type> type##Map;          \
  I3_POINTER_TYPEDEFS(type##Map);

//These all have to be defined separately because they don't work with templates
DO_DEFINE_IC(AntennaTimeSeries);
DO_DEFINE_IC(AntennaSpectrum);
DO_DEFINE_IC(EFieldTimeSeries);
DO_DEFINE_IC(EFieldSpectrum);

#undef DO_DEFINE_IC



/////////////////////////////////////////////////////
//////////////Batch definitions of operators/////////
/////////////////////////////////////////////////////

// I3AntennaWaveform<T> + - I3AntennaWaveform<T>
#define INNER_ARITHMETIC_BINARY_OPERATOR(_op_, _stl_functor_)                         \
  template<typename T>                                                                \
  I3AntennaWaveform<T>                                                                \
  I3AntennaWaveform<T>::operator _op_(const I3AntennaWaveform<T>& trace)              \
    const                                                                             \
  {                                                                                   \
    const SizeType size = GetSize();                                                  \
    if (IsSameTypeAs(trace)) {                                                        \
      I3AntennaWaveform<T> result(size, binning_);                                    \
      transform(dataArray_.begin(), dataArray_.end(), trace.Begin(), result.Begin(),  \
                _stl_functor_<T>());                                                  \
      result.binning_ = binning_;                                                     \
      result.offset_ = offset_;                                                       \
      return result;                                                                  \
    } else {                                                                          \
      log_warn("Trying to do operation on traces where size and binning are unequal");\
      I3AntennaWaveform<T> foo;                                                       \
      return foo;                                                                     \
    }                                                                                 \
  }

INNER_ARITHMETIC_BINARY_OPERATOR(+, std::plus)
INNER_ARITHMETIC_BINARY_OPERATOR(-, std::minus)

#undef INNER_ARITHMETIC_BINARY_OPERATOR

// I3AntennaWaveform<T> += -= I3AntennaWaveform<T>
#define INNER_AUGMENTED_ARITHMETIC_OPERATOR(_op_, _stl_functor_)                        \
  template<typename T>                                                                  \
  I3AntennaWaveform<T>&                                                                 \
  I3AntennaWaveform<T>::operator _op_(const I3AntennaWaveform<T>& trace)                \
  {                                                                                     \
    if (IsSameTypeAs(trace)) {                                                          \
      transform(dataArray_.begin(), dataArray_.end(), trace.Begin(), dataArray_.begin(),\
                _stl_functor_<T>());                                                    \
    } else  {                                                                           \
      log_warn("Trying to do operation on traces where size and binning are unequal");  \
    }                                                                                   \
    return *this;                                                                       \
  }

INNER_AUGMENTED_ARITHMETIC_OPERATOR( +=, std::plus)
INNER_AUGMENTED_ARITHMETIC_OPERATOR( -=, std::minus)

#undef INNER_AUGMENTED_ARITHMETIC_OPERATOR



// I3AntennaWaveform<T> += -= T
#define OUTER_AUGMENTED_ARITHMETIC_OPERATOR(_op_, _stl_functor_)               \
  template<typename T>                                                         \
  I3AntennaWaveform<T>&                                                        \
  I3AntennaWaveform<T>::operator _op_(const T value)                           \
  {                                                                            \
    transform(dataArray_.begin(), dataArray_.end(), dataArray_.begin(),        \
              bind(_stl_functor_<T>(), std::placeholders::_1, value));         \
    return *this;                                                              \
  }

OUTER_AUGMENTED_ARITHMETIC_OPERATOR( +=, std::plus)
OUTER_AUGMENTED_ARITHMETIC_OPERATOR( -=, std::minus)

#undef OUTER_AUGMENTED_ARITHMETIC_OPERATOR

#define LEFT_AUGMENTED_ARITHMETIC_OPERATOR_DOUBLE(_op_)              \
  template<typename T>                                               \
  I3AntennaWaveform<T>&                                              \
  I3AntennaWaveform<T>::operator _op_(const double value) {          \
    for (unsigned int ibin = 0; ibin < GetSize(); ibin++) {          \
        dataArray_[ibin] _op_ value;                                 \
    }                                                                \
    return *this;                                                    \
  }

LEFT_AUGMENTED_ARITHMETIC_OPERATOR_DOUBLE( *= )
LEFT_AUGMENTED_ARITHMETIC_OPERATOR_DOUBLE( /= )

#undef LEFT_AUGMENTED_ARITHMETIC_OPERATOR_DOUBLE

#define LEFT_ARITHMETIC_OPERATOR(_op_)                               \
  template<typename T>                                               \
  I3AntennaWaveform<T>                                               \
  I3AntennaWaveform<T>::operator _op_(const double value) const {    \
    I3AntennaWaveform<T> newTrace(*this);                            \
    for (unsigned int ibin = 0; ibin < newTrace.GetSize(); ibin++) { \
        newTrace[ibin] = newTrace[ibin] _op_ value;                  \
    }                                                                \
    return newTrace;                                                 \
  }

LEFT_ARITHMETIC_OPERATOR( * )
LEFT_ARITHMETIC_OPERATOR( / )

#undef LEFT_ARITHMETIC_OPERATOR

AntennaTimeSeries & operator *=(AntennaTimeSeries &lhs, const AntennaTimeSeries &rhs);
AntennaTimeSeries & operator /=(AntennaTimeSeries &lhs, const AntennaTimeSeries &rhs);
AntennaSpectrum & operator *=(AntennaSpectrum &lhs, const AntennaSpectrum &rhs);
AntennaSpectrum & operator /=(AntennaSpectrum &lhs, const AntennaSpectrum &rhs);

AntennaTimeSeries operator *(AntennaTimeSeries &lhs, const AntennaTimeSeries &rhs);
AntennaTimeSeries operator /(AntennaTimeSeries &lhs, const AntennaTimeSeries &rhs);
AntennaSpectrum operator *(AntennaSpectrum &lhs, const AntennaSpectrum &rhs);
AntennaSpectrum operator /(AntennaSpectrum &lhs, const AntennaSpectrum &rhs);

#endif
