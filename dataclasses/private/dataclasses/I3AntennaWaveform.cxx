#include <dataclasses/I3AntennaWaveform.h>

#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/complex.hpp>


template<typename T>
template <class Archive>
void I3AntennaWaveform<T>::serialize(Archive& ar, unsigned version) {
  if (version > i3antennawaveform_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AntennaWaveform class.", version, i3antennawaveform_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  ar & make_nvp("Array", dataArray_);
  ar & make_nvp("Binning", binning_);
  ar & make_nvp("Offset", offset_);
}

//Because * is the dot product for IPosition, only the 1D cases
//have the self*=self
#define LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D(_type_, _op_)          \
  _type_ & operator _op_(_type_ &lhs, const _type_ &rhs) {           \
    for (unsigned int ibin = 0; ibin < lhs.GetSize(); ibin++) {      \
        lhs[ibin] _op_ rhs[ibin];                                    \
    }                                                                \
    return lhs;                                                      \
  }

LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D(AntennaTimeSeries, *=)
LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D(AntennaTimeSeries, /=)
LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D(AntennaSpectrum, *=)
LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D(AntennaSpectrum, /=)

#undef LEFT_AUGMENTED_ARITHMETIC_OPERATOR_1D

//Because * is the dot product for IPosition, only the 1D cases
//have the self*self
#define LEFT_ARITHMETIC_OPERATOR_1D(_type_, _op_)          \
  _type_ operator _op_(_type_ &lhs, const _type_ &rhs) {             \
    _type_ output(lhs);                                              \
    for (unsigned int ibin = 0; ibin < lhs.GetSize(); ibin++) {      \
        output[ibin] = lhs[ibin] _op_ rhs[ibin];                     \
    }                                                                \
    return output;                                                   \
  }

LEFT_ARITHMETIC_OPERATOR_1D(AntennaTimeSeries, *)
LEFT_ARITHMETIC_OPERATOR_1D(AntennaTimeSeries, /)
LEFT_ARITHMETIC_OPERATOR_1D(AntennaSpectrum, *)
LEFT_ARITHMETIC_OPERATOR_1D(AntennaSpectrum, /)

#undef LEFT_ARITHMETIC_OPERATOR_1D


I3_SERIALIZABLE(AntennaTimeSeries);
I3_SERIALIZABLE(AntennaSpectrum);
I3_SERIALIZABLE(EFieldTimeSeries);
I3_SERIALIZABLE(EFieldSpectrum);

I3_SERIALIZABLE(AntennaTimeSeriesMap);
I3_SERIALIZABLE(AntennaSpectrumMap);
I3_SERIALIZABLE(EFieldTimeSeriesMap);
I3_SERIALIZABLE(EFieldSpectrumMap);