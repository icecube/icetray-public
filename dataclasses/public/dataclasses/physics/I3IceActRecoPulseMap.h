#ifndef I3IceActRecoPulseMap_INCLUDE_H
#define I3IceActRecoPulseMap_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/IceActKey.h>
#include <dataclasses/physics/I3RecoPulse.h>

typedef I3Map<unsigned, I3RecoPulseSeries> I3IceActRecoPulseSeries;
typedef I3Map<IceActKey, I3IceActRecoPulseSeries > I3IceActRecoPulseSeriesMap;  //Cluster of ChannelMaps

I3_POINTER_TYPEDEFS(I3IceActRecoPulseSeries);
I3_POINTER_TYPEDEFS(I3IceActRecoPulseSeriesMap);

#endif
