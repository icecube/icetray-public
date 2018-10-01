/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3CalibrationService.h
 * @version $Revision$
 * @date $Date$
 * @author pretz
 */

#ifndef INTERFACES_I3CALIBRATIONSERVICE_H_INCLUDED
#define INTERFACES_I3CALIBRATIONSERVICE_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3DefaultName.h>

class I3Calibration;
I3_POINTER_TYPEDEFS(I3Calibration);

class I3Time;

/**
 * @brief Produces I3Calibration objects given a time.
 * Then it tells the next time the calibration will change.
 */
class I3CalibrationService
{
 public:

  /**
   * @brief Gets the calibration and the header associated with the given time
   * It is expected that the 'start' and 'end' times of the data
   * header will be appropirately filled.
   */
  virtual I3CalibrationConstPtr GetCalibration(I3Time time) = 0;

  virtual ~I3CalibrationService();

};

I3_DEFAULT_NAME(I3CalibrationService);
I3_POINTER_TYPEDEFS(I3CalibrationService);

#endif
