////////////////////////////////////////////////////////////
//Implemented by: Alan Coleman
//Base on FFTDataContainer.h in Pierre Auger's Offline Framework)
//Generic template class to automatically handle the time/freq data so they are always up to date
////////////////////////////////////////////////////////////
//
//This function holds two traces, one with the time series and one with the frequency spectrum
//If you ask for either of these (GetTimeSeries or GetFrequencySpectrum) then it will assume that
//you intend to augment them and will do a FFT on the other trace before giving it to you next time.
//To save CPU cycles, if you aren't going to augment the trace when you ask for it, instead use
//GetConstTimeSeries or GetConstFrequencySpectrum


#ifndef FFTDATACONTAINER_INCLUDE_H
#define FFTDATACONTAINER_INCLUDE_H

#include <complex>
#include <iostream>

#include <dataclasses/Utility.h>
#include <icetray/I3Logging.h>

#include <dataclasses/I3AntennaWaveform.h>

static const unsigned int i3fftdatacontainer_version_ = 0;

template<typename T, typename F>
class FFTDataContainer : public I3FrameObject {

 public:
  using SizeType = typename std::vector<T>::size_type;

  FFTDataContainer()
    : upToDateDomain_(Both), nyquistZone_(1u) {
  }

  FFTDataContainer(const FFTDataContainer<T, F>& data) {

    LoadTimeSeries(data.GetConstTimeSeries());
    LoadFrequencySpectrum(data.GetConstFrequencySpectrum());

    upToDateDomain_ = data.upToDateDomain_;
    nyquistZone_ = data.nyquistZone_;
  }

  FFTDataContainer(const I3AntennaWaveform<T>& timeSeries) : nyquistZone_(1u) {
    LoadTimeSeries(timeSeries);
  }

  FFTDataContainer(const I3AntennaWaveform<F>& spectrum) : nyquistZone_(1u) {
    LoadFrequencySpectrum(spectrum);
  }

  virtual ~FFTDataContainer() {}

  ////////////////////////////////
  ////////Setters/Getters/////////
  ////////////////////////////////

  unsigned int GetNyquistZone() const {return nyquistZone_;}
  void SetNyquistZone(unsigned int zone) {
    if (zone < 1) {
      log_fatal("Requested a nyquist zone less than 1. Not great.");
    }
    nyquistZone_ = zone;
  }

  //Get the frequency of the requested bin
  double GetFrequencyOfBin(typename I3AntennaWaveform<F>::SizeType bin) const {
    if (upToDateDomain_ == Time) {
      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateFrequencySpectrum();
    }
    double binning = frequencySpectrum_.GetBinning();
    typename I3AntennaWaveform<F>::SizeType highestbin = frequencySpectrum_.GetSize() - 1;
    double bandwidth = static_cast<double>(highestbin) * binning;
    unsigned int evenedzone = (nyquistZone_ / 2) * 2; //Round down to nearest 2
    double startingfrequency = evenedzone * bandwidth;
    int sign = 1;
    if (nyquistZone_ == evenedzone) {
      sign = -1; // we are in an even Nyquist zone, i.e., the frequencies count downwards
    }
    return startingfrequency + sign * static_cast<double>(bin) * binning;
  }

  //Get the bin corresponding to the requested frequency (rounded down)
  int GetBinOfFrequency(double frequency) const {
    if (upToDateDomain_ == Time) {
      // we have to update the cached frequency spectrum data, so we need to cast away the constness of the this-object
      // this is ok since the change is "invisible" to the user, who always gets up-to-date data
      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateFrequencySpectrum(); // do the fft
    }
    double binning = frequencySpectrum_.GetBinning();
    typename I3AntennaWaveform<F>::SizeType highestbin = frequencySpectrum_.GetSize() - 1;
    double bandwidth = static_cast<double>(highestbin) * binning;
    if ( ((nyquistZone_ - 1)*bandwidth > frequency) or (nyquistZone_ * bandwidth < frequency) ) {
      log_fatal("Requested bin for a frequency that is not in the FrequencySpectrum.");
    }

    unsigned int evenedzone = (nyquistZone_ / 2) * 2;
    double startingfrequency = evenedzone * bandwidth;
    int sign = 1;
    if (nyquistZone_ == evenedzone) {
      sign = -1; // we are in an even Nyquist zone, i.e., the frequencies count downwards
    }
    double timeBinning = timeSeries_.GetBinning();
    int timeTraceLength = timeSeries_.GetSize();

    double bin = (frequency - startingfrequency) * sign *  timeBinning * timeTraceLength;
    return static_cast<int>(std::floor(0.5 + bin));
  }

  /// read out the time series (read access)
  const I3AntennaWaveform<T>& GetConstTimeSeries() const {
    if (upToDateDomain_ == Frequency) {
      // we have to update the cached time series data, so we need to cast away the constness of the this-object
      // this is ok since the change is "invisible" to the user, who always gets up-to-date data
      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateTimeSeries(); // do the fft^-1
    }
    return timeSeries_;
  }

  /// read out the time series (write access)
  I3AntennaWaveform<T>& GetTimeSeries() {

    if (upToDateDomain_ == Frequency) {
      UpdateTimeSeries(); // do the fft^-1
    }
    upToDateDomain_ = Time;  // write access is possible, so have to assume data will change
    return timeSeries_;
  }

  I3AntennaWaveform<T> GetTimeSeriesPython() {
    if (upToDateDomain_ == Frequency) {
      // we have to update the cached time series data, so we need to cast away the constness of the this-object
      // this is ok since the change is "invisible" to the user, who always gets up-to-date data
      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateTimeSeries(); // do the fft^-1
    }
    return timeSeries_;
  }

  /// read out the frequency spectrum (read access)
  const I3AntennaWaveform<F>& GetConstFrequencySpectrum() const {
    if (upToDateDomain_ == Time) {
      // we have to update the cached frequency spectrum data, so we need to cast away the constness of the this-object
      // this is ok since the change is "invisible" to the user, who always gets up-to-date data
      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateFrequencySpectrum(); // do the fft
    }
    return frequencySpectrum_;
  }

  /// read out the frequency spectrum (write access)
  I3AntennaWaveform<F>& GetFrequencySpectrum() {

    if (upToDateDomain_ == Time) {

      UpdateFrequencySpectrum(); // do the fft
    }
    upToDateDomain_ = Frequency;  // write access is possible, so have to assume data will change
    return frequencySpectrum_;
  }

  const I3AntennaWaveform<F> GetFrequencySpectrumPython() const {
    if (upToDateDomain_ == Time) {
      // we have to update the cached frequency spectrum data, so we need to cast away the constness of the this-object
      // this is ok since the change is "invisible" to the user, who always gets up-to-date data

      FFTDataContainer<T, F>* mutablethis = const_cast<FFTDataContainer<T, F>*>(this);
      mutablethis->UpdateFrequencySpectrum(); // do the fft
    }
    return frequencySpectrum_;
  }

  void LoadTimeSeries(const I3AntennaWaveform<T> &trace) {
    timeSeries_.Clear();
    upToDateDomain_ = Time;
    for (unsigned int ibin = 0; ibin < trace.GetSize(); ibin++) {
      timeSeries_.PushBack(trace[ibin]);
    }

    if (std::isnan(trace.GetBinning()))
      log_warn("About to load a time series with NAN binning, careful!");
    if (std::isnan(trace.GetOffset()))
      log_warn("About to load a time series with NAN offset, careful!");

    timeSeries_.SetBinning(trace.GetBinning());
    timeSeries_.SetOffset(trace.GetOffset());
  }

  void LoadFrequencySpectrum(const I3AntennaWaveform<F> &spec) {
    frequencySpectrum_.Clear();
    upToDateDomain_ = Frequency;
    for (unsigned int ibin = 0; ibin < spec.GetSize(); ibin++) {
      frequencySpectrum_.PushBack(spec[ibin]);
    }

    if (NAN == spec.GetBinning())
      log_warn("About to load a frequency spectrum with NAN binning, careful!");

    frequencySpectrum_.SetBinning(spec.GetBinning());
    frequencySpectrum_.SetOffset(spec.GetOffset());
  }

  //Function which removes extra data for hard-disk storage
  void PurgeData() {
    if (Frequency == upToDateDomain_) {
      timeSeries_.Clear();
    } else if (Time == upToDateDomain_) {
      frequencySpectrum_.Clear();
    } else {//Both are up to date
      frequencySpectrum_.Clear();
      upToDateDomain_ = Time;
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const FFTDataContainer <T, F> &rhs) {
    os << "FFTDataContainer( TimeSeries of length " << rhs.timeSeries_.GetSize()
       << " and FreqSpec of length " << rhs.frequencySpectrum_.GetSize() << " )";
    return os;
  }


 private:
  enum eDomain {Time, Frequency, Both};
  eDomain upToDateDomain_;

  unsigned int nyquistZone_;

  I3AntennaWaveform<T> timeSeries_;
  I3AntennaWaveform<F> frequencySpectrum_;

  //Specializations defined in .cxx file
  void UpdateTimeSeries();
  void UpdateFrequencySpectrum();


  friend class icecube::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, unsigned int version);

};

using FFTData = FFTDataContainer<double, std::complex<double> >;
using FFTData3D = FFTDataContainer<I3Position, I3ComplexVector>;

#define DO_ICECUBE_DEFS(type)                         \
  I3_CLASS_VERSION(type, i3fftdatacontainer_version_);\
  I3_POINTER_TYPEDEFS(type);                          \
  using type##Map = I3Map<AntennaKey, type>;               \
  I3_POINTER_TYPEDEFS(type##Map);                     \

//These all have to be defined separately because they don't work with templates
DO_ICECUBE_DEFS(FFTData);
DO_ICECUBE_DEFS(FFTData3D);

#undef DO_ICECUBE_DEFS

#endif
