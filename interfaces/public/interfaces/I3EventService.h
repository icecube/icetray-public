/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3EventService.h
 * @version $Revision$
 * @date $Date$
 */

#ifndef INTERFACES_I3EVENTSERVICE_H_INCLUDED
#define INTERFACES_I3EVENTSERVICE_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>

class I3Frame;

class I3Time;

/**
 * @brief This is the interface to classes which  
 * serve up an event and the time of the next 
 * event.  This is intended to be an icetray service
 */
class I3EventService
{
 public:

  /**
   * @brief indicates whether or not there are more events to find.
   * 
   */
  virtual bool MoreEvents() = 0;

  /**
   * @brief This method should fill the frame with all the relevent
   * frame data.  Returned is the time of the event.
   */
  virtual I3Time PopEvent(I3Frame& frame) = 0;

  /**
   * @brief This method should delete any keys that have not changed
   * since the frame was injected into the module chain, i.e. turn
   * the frame into a diff.
   */
  virtual void DiffEvent(I3Frame& frame) { return; }
  virtual bool DiffEventImplemented() { return false; }

  virtual ~I3EventService();
};

I3_DEFAULT_NAME(I3EventService);
I3_POINTER_TYPEDEFS(I3EventService);

#endif
