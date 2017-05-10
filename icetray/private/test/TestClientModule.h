/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file TestClientModule.h
 * @version $Revision$
 * @date $Date$
 * @author pretz
 */

#ifndef TESTCLIENTMODULE_H
#define TESTCLIENTMODULE_H

#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Module.h"
#include "I3ModuleTestConfig.h"

/**
 * @brief This is a template for the module which sits at the end of the 
 * chain and verifies that everything made it through.
 * 
 * The module inherits from the template parameter Type so that
 * in 'Type' users can put anything that they want access to when
 * deciding whether or not the frame content is correct
 *
 * @todo put some helper methods like in I3PhysicsModule here.
 */
template <class Type>
class TestClientModule : public I3Module, public Type
{
 private:
  static const int max_frames = I3ModuleTestConfig::max_frames;

  /** a pointer to the next 'DoNextFrame' function to call */
  void (TestClientModule::*nextFrame)(I3FramePtr);
 public:
  /**
   * standard icetray module constructor
   */
  TestClientModule(const I3Context& context) : I3Module(context)
    {
      nextFrame = &TestClientModule::template DoNextFrame<0>;
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void Physics(I3FramePtr frame)
    {
      HandleFrame(frame);
      PushFrame(frame);
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void Geometry(I3FramePtr frame)
    {
      HandleFrame(frame);
      PushFrame(frame);
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void Calibration(I3FramePtr frame)
    {
      HandleFrame(frame);
      PushFrame(frame);
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void DetectorStatus(I3FramePtr frame)
    {
      HandleFrame(frame);
      PushFrame(frame);
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void Monitor(I3FramePtr frame)
    {
      HandleFrame(frame);
      PushFrame(frame);
    }

  /**
   * @brief Kinda a hack.  Should just override Process()
   * @todo rework this to just override Process()
   */
  void HandleFrame(I3FramePtr frame)
    {
      (this->*nextFrame)(frame);
    }

  /**
   * @brief Handles calling the right function to check the frame
   * Then schedules the next function for calling.
   */
  template <int i>
    void DoNextFrame(I3FramePtr frame)
    {
      CheckFrame<i>(frame);
      if(i <= max_frames)
	nextFrame = &TestClientModule::template DoNextFrame<i<=max_frames ? i+1 : -1>;
    }

  /**
   * This is the template method that actually performs the check on the 
   * frame.
   */
  template<int i>
    void CheckFrame(I3FramePtr frame)
    {
    }
};

#endif
