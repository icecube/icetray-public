#ifndef I3IceActWaveformMap_INCLUDE_H
#define I3IceActWaveformMap_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/IceActKey.h>
#include <dataclasses/physics/I3Waveform.h>


typedef I3Map<unsigned, std::vector<I3Waveform>> I3IceActWaveform;
typedef I3Map<IceActKey, I3IceActWaveform> I3IceActWaveformMap;
I3_POINTER_TYPEDEFS(I3IceActWaveform);
I3_POINTER_TYPEDEFS(I3IceActWaveformMap);

#endif
