// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <icetray/I3PODHolder.h>
#include <icetray/serialization.h>

typedef I3PODHolder<uint64_t> I3UInt64;
I3_SERIALIZABLE(I3UInt64);
