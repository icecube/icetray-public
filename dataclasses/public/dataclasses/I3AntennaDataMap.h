////////////////////////////////////////////////////////////
//Author: Alan Coleman
//Container to hold the antenna channels
////////////////////////////////////////////////////////////
//
//Each antenna has an associated I3ChannelMap which can be called
//using its AntennaKey. Each I3ChannelMap will have one FFTDataContainer
//for each of its channels.
//All trace voltage information should be stored as I3AntennaDataMap.

#ifndef I3ANTENNADATAMAP_INCLUDE_H
#define I3ANTENNADATAMAP_INCLUDE_H

#include <dataclasses/I3Map.h>
#include <dataclasses/I3AntennaChannel.h>

typedef I3Map<AntennaKey, I3AntennaChannelMap> I3AntennaDataMap;  //Cluster of ChannelMaps
I3_POINTER_TYPEDEFS(I3AntennaDataMap);

#endif
