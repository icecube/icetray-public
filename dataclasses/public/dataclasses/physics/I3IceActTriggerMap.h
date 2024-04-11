// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef I3IceActWaveformMap_INCLUDE_H
#define I3IceActWaveformMap_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/IceActKey.h>
#include <dataclasses/physics/I3Trigger.h>


typedef I3Map<IceActKey, I3Trigger> I3IceActTriggerMap;
I3_POINTER_TYPEDEFS(I3IceActTriggerMap);

#endif
