#include <dataclasses/I3AntennaChannel.h>
#include <dataclasses/fft/FFTHilbertEnvelope.h>

#include <icetray/I3Units.h>

I3_SERIALIZABLE(I3AntennaChannel);
I3_SERIALIZABLE(I3AntennaChannelMap);

I3AntennaChannel::I3AntennaChannel():
  maxAmplitude_(NAN), timeAtMax_(NAN), isUpToDate_(true) {
}

I3AntennaChannel::I3AntennaChannel(const I3AntennaChannel& chData) {
  fftData_ = FFTData(chData.fftData_);
  maxAmplitude_ = chData.maxAmplitude_;
  timeAtMax_ = chData.timeAtMax_;
  isUpToDate_ = false; //Just to be safe
  UpdateValues();
}

I3AntennaChannel::I3AntennaChannel(const FFTData& fftData) {
  fftData_ = FFTData(fftData);
  maxAmplitude_ = NAN;
  timeAtMax_ = NAN;
  isUpToDate_ = false;
  UpdateValues();
}

FFTData& I3AntennaChannel::GetFFTData() {
  isUpToDate_ = false;
  return fftData_;
}

const FFTData& I3AntennaChannel::GetConstFFTData() const {
  return fftData_;
}

void I3AntennaChannel::UpdateValues() const {
  if (! isUpToDate_) {
    //Remove the constness to update the values
    I3AntennaChannel* mutablethis = const_cast<I3AntennaChannel*>(this);

    mutablethis->timeAtMax_ = fft::GetHilbertPeakTime(mutablethis->fftData_, mutablethis->maxAmplitude_);

    mutablethis->isUpToDate_ = true;
  }
}

double I3AntennaChannel::GetMaxAmplitude() const {
  UpdateValues();
  return maxAmplitude_;
}

double I3AntennaChannel::GetTimeAtMaximum() const {
  UpdateValues();
  return timeAtMax_;
}

template <class Archive>
void
I3AntennaChannel::serialize(Archive& ar, unsigned version) {
  if (version > i3antennachannel_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AntennaChannel class.", version, i3antennachannel_version_);

  maxAmplitude_ = NAN;
  timeAtMax_ = NAN;
  isUpToDate_ = false;
  fftData_.PurgeData();

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("MaxAmplitude", maxAmplitude_);
  ar & make_nvp("TimeAtMaximum", timeAtMax_);
  ar & make_nvp("IsUpToDate", isUpToDate_);
  ar & make_nvp("FFTData", fftData_);
}

std::ostream& I3AntennaChannel::Print(std::ostream& os) const {
  UpdateValues();
  os << "[I3AntennaChannel IsUpToDate: " << (isUpToDate_ ? "True" : "False") << '\n' 
     << "                MaxAmplitude: " << maxAmplitude_ / (I3Units::volt * 1.e-3) << "[mV]" << '\n'
     << "               TimeAtMaximum: " << timeAtMax_ / I3Units::nanosecond << "[ns]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaChannel& g) {
  return (g.Print(os));
}