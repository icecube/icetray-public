#ifndef I3ScintRecoPulseSeriesMap_INCLUDE_H
#define I3ScintRecoPulseSeriesMap_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/ScintKey.h>
#include <dataclasses/physics/I3RecoPulse.h>

typedef I3Map<ScintKey, I3RecoPulseSeries > I3ScintRecoPulseSeriesMap;
I3_POINTER_TYPEDEFS(I3ScintRecoPulseSeriesMap);

#endif
