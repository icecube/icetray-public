// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef I3IceActMaps_INCLUDE_H
#define I3IceActMaps_INCLUDE_H

#include "icetray/I3Bool.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/I3String.h"
#include "dataclasses/I3Vector.h"
#include "dataclasses/IceActKey.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/physics/I3Trigger.h"

typedef I3Map<IceActKey, bool> I3IceActBoolMap;
typedef I3Map<IceActKey, int32_t> I3IceActIntMap;
typedef I3Map<IceActKey, uint32_t> I3IceActUnsignedMap;
typedef I3Map<IceActKey, double> I3IceActDoubleMap;
typedef I3Map<IceActKey, I3String > I3IceActStringMap;

typedef I3Map<IceActKey, I3MapUnsignedUnsigned> I3IceActTelescopePixelUnsignedMap;
typedef I3Map<IceActKey, I3MapUnsignedDouble> I3IceActTelescopePixelDoubleMap;
typedef I3Map<IceActKey, I3MapIntVectorInt> I3IceActTelescopePixelVectorIntMap;

typedef I3Map<IceActKey, I3MapStringDouble> I3IceActTelescopeStringDoubleMap;

typedef I3Map<IceActKey, I3Trigger> I3IceActTriggerMap;

typedef I3Map<unsigned, std::vector<I3Waveform>> I3IceActWaveform;
typedef I3Map<IceActKey, I3IceActWaveform> I3IceActWaveformMap;

typedef I3Map<unsigned, I3RecoPulseSeries> I3IceActRecoPulseSeries;
typedef I3Map<IceActKey, I3IceActRecoPulseSeries> I3IceActRecoPulseSeriesMap;

I3_POINTER_TYPEDEFS(I3IceActBoolMap);
I3_POINTER_TYPEDEFS(I3IceActIntMap);
I3_POINTER_TYPEDEFS(I3IceActUnsignedMap);
I3_POINTER_TYPEDEFS(I3IceActDoubleMap);
I3_POINTER_TYPEDEFS(I3IceActStringMap);

I3_POINTER_TYPEDEFS(I3IceActTelescopePixelUnsignedMap);
I3_POINTER_TYPEDEFS(I3IceActTelescopePixelDoubleMap);
I3_POINTER_TYPEDEFS(I3IceActTelescopePixelVectorIntMap);

I3_POINTER_TYPEDEFS(I3IceActTelescopeStringDoubleMap);

I3_POINTER_TYPEDEFS(I3IceActTriggerMap);

I3_POINTER_TYPEDEFS(I3IceActWaveform);
I3_POINTER_TYPEDEFS(I3IceActWaveformMap);

I3_POINTER_TYPEDEFS(I3IceActRecoPulseSeries);
I3_POINTER_TYPEDEFS(I3IceActRecoPulseSeriesMap);

#endif
