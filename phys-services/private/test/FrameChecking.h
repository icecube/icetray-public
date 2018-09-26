#ifndef FRAMECHECKING_H
#define FRAMECHECKING_H

#include "dataclasses/geometry/I3Geometry.h"

#include "dataclasses/calibration/I3Calibration.h"

#include "dataclasses/status/I3DetectorStatus.h"

#include "dataclasses/physics/I3EventHeader.h"

#include "icetray/I3Frame.h"

// icetray doesn't normally allow you to get the string for the 
// stream that your frame is on.  Had to hack in a way to do it.
inline
string DumpStop(I3Frame& frame)
{
  vector<string> possibleNames;
  possibleNames.push_back("Geometry");
  possibleNames.push_back("DetectorStatus");
  possibleNames.push_back("Calibration");
  possibleNames.push_back("Physics");
  possibleNames.push_back("Monitor");
  
  for(vector<string>::iterator iter = possibleNames.begin() ; 
      iter != possibleNames.end() ;
      iter++)
    {
      if(!I3Stream::StreamExists(*iter))
	continue;
      else if(frame.GetStop() == I3Stream::FindStream(*iter))
	return *iter;
    }
  return "UNKNOWN STREAM";
   
}

inline bool GeometryPresent(I3Frame& frame)
{
  if(frame.Get<I3GeometryPtr>())
    return true;
  return false;
}

inline bool EventPresent(I3Frame& frame)
{
  return true;
}

inline bool CalibrationPresent(I3Frame& frame)
{
  if(frame.Get<I3CalibrationPtr>())
    return true;
  return false;
}

inline bool DetectorStatusPresent(I3Frame& frame)
{
  if(frame.Get<I3DetectorStatusPtr>())
    return true;
  return false;
}

#endif
