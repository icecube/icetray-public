/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3GeometryService.h
 * @version $Revision$
 * @date $Date$
 */

#ifndef INTERFACES_I3GEOMETRYSERVICE_H_INCLUDED
#define INTERFACES_I3GEOMETRYSERVICE_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>

I3_FORWARD_DECLARATION(I3Geometry);

class I3Time;

/**
 * @brief This is an interface to classes which generates I3Geometry and
 * I3GeometryHeader objects given an I3Time.
 * This is intended to be an icetray services for getting the geometry
 * information into the system
 */
class I3GeometryService
{
 public:

  /**
   * @brief gives the I3Geometry and I3GeometryHeader for the
   * indicated time.
   */
  virtual I3GeometryConstPtr GetGeometry(I3Time time) = 0;

  virtual ~I3GeometryService();

};

I3_DEFAULT_NAME(I3GeometryService);
I3_POINTER_TYPEDEFS(I3GeometryService);


#endif
