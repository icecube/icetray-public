#ifndef I3ANTENNACHANNEL_INCLUDE_H
#define I3ANTENNACHANNEL_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/fft/FFTDataContainer.h>

static const unsigned int i3antennachannel_version_ = 0;

class I3AntennaChannel : public I3FrameObject {
 public:
  I3AntennaChannel();
  I3AntennaChannel(const I3AntennaChannel& antData);
  I3AntennaChannel(const FFTData& fftData);
  ~I3AntennaChannel() {}

  double GetMaxAmplitude() const;
  double GetTimeAtMaximum() const;

  FFTData& GetFFTData();
  const FFTData& GetConstFFTData() const;

  std::ostream& Print(std::ostream&) const; //Required from base class

 private:
  double maxAmplitude_;
  
  double timeAtMax_;

  bool isUpToDate_;

  FFTData fftData_;

  void UpdateValues() const;

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3AntennaChannel&);

I3_POINTER_TYPEDEFS(I3AntennaChannel);
I3_CLASS_VERSION(I3AntennaChannel, i3antennachannel_version_);

typedef I3Map<int, I3AntennaChannel> I3AntennaChannelMap; //Map of channel index and I3AntennaChannel
I3_POINTER_TYPEDEFS(I3AntennaChannelMap);

#endif
