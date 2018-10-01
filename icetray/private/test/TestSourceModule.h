/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: TestSourceModule.h 15950 2006-02-17 18:49:26Z troy $
 *
 * @file TestSourceModule.h
 * @version $Revision: 1.1 $
 * @date $Date: 2006-02-17 19:49:26 +0100 (Fri, 17 Feb 2006) $
 * @author pretz
 */

#ifndef TESTSOURCEMODULE_H
#define TESTSOURCEMODULE_H

#include "icetray/I3TrayHeaders.h"
#include "I3ModuleTestConfig.h"
#include <cassert>

/**
 * @brief This is the template for the module that puts frames into the 
 * system.  
 *
 * The module inherits from the template parameter Type so that 
 * people can put into 'Type' anthing they want while building frames
 *
 * @todo put some 'helper' methods like in I3PhysicsModule here.
 */
template <class Type>
class TestSourceModule : public I3Module, public Type
{
 private:
  static const int max_frames = I3ModuleTestConfig::max_frames;

  /** The next 'DoNextFrame()' method to be called*/
  I3FramePtr (TestSourceModule::*nextFrame)();
 public:
  /**
   * The standard icetray source module constructor
   */
  TestSourceModule(I3Context& context) : I3Module(context)
    {
      nextFrame = &TestSourceModule::template DoNextFrame<0> ;

      AddOutBox("OutBox");
    }

  /**
   * overridding the 'Process()' transition as all IceTray sources
   * must
   */
  void Process()
    {
      I3FramePtr frame =  (this->*nextFrame)();
      
      PushFrame(frame,"OutBox");
    }

  /**
   * Dispatcheds the 'i'th frame and then schedules the 'i+1'th method
   * for calling.
   */
  template <int i>
    I3FramePtr DoNextFrame()
    {
      I3FramePtr frame = MakeTestFrame<i>();
      if(i <= max_frames)
	{
	  nextFrame = &TestSourceModule::template DoNextFrame<i<=max_frames ? i+1 : -1>;
	}
      return frame;
    }

  /**
   * This is the method that users should specialize.  
   *
   * @todo should not implement this function since it's expected to be 
   * specialized.
   */
  template<int i>
    I3FramePtr MakeTestFrame()
    {
      I3FramePtr frame(new I3Frame(I3Frame::Geometry));
      log_info("Requesting suspension");
      RequestSuspension();
      return frame;
    }
};

#endif
