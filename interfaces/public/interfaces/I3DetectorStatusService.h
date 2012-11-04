/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3DetectorStatusService.h
 * @version $Revision$
 * @date $Date$
 * @author pretz
 */

#ifndef ICETRAY_I3DETECTORSTATUSSERVICE_H_INCLUDED
#define ICETRAY_I3DETECTORSTATUSSERVICE_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3DefaultName.h>

I3_FORWARD_DECLARATION(I3DetectorStatus);

class I3Time;

/**
 * @brief Produces I3DetectorStatus and I3DetectorStatus Header
 * objects given a I3Time.  Intended to be an icetray service 
 * which provides the detector status information
 */
class I3DetectorStatusService
{
 public:

  virtual I3DetectorStatusConstPtr GetDetectorStatus(I3Time time) = 0;

  virtual ~I3DetectorStatusService();
};

I3_DEFAULT_NAME(I3DetectorStatusService);
I3_POINTER_TYPEDEFS(I3DetectorStatusService);

#endif
