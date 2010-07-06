/**
 *  $Id: I3Module.h 49585 2008-10-06 17:23:28Z blaufuss $
 *  
 *  Copyright (C) 2007, 2008
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#ifndef ICETRAY_I3MODULE_H_INCLUDED
#define ICETRAY_I3MODULE_H_INCLUDED

#include <cstdlib>
#include <string>
#include <set>
#include <icetray/Version.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Context.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3PhysicsUsage.h>
#include <boost/type_traits/is_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/python/object.hpp>
#include <boost/python/extract.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

class I3Configuration;
class I3Context;
class I3Frame;

using namespace std;

/**
 * This class defines the interface which should be implementaed by all 
 * modules in IceTray.
 *
 * @version $Id: I3Module.h 49585 2008-10-06 17:23:28Z blaufuss $
 */
class I3Module;
I3_POINTER_TYPEDEFS(I3Module);

class I3Module
{
  friend class I3Tray;

  typedef std::deque<I3FramePtr> FrameFifo;
  I3_POINTER_TYPEDEFS(FrameFifo);

  typedef map<string, pair<FrameFifoPtr, I3ModulePtr> > outboxmap_t;

  typedef map<I3Frame::Stream, boost::function<void(I3FramePtr)> > methods_t;
  
public:

  /**
   * Constructor.
   *
   * @param context the I3Context object containing this objects
   * services, configuration, outboxes and so forth.  Since a module
   * is useless without a context, no default constructor is provided.
   */
  I3Module(const I3Context& context);

  /**
   * Destructor.
   */
  virtual ~I3Module();

  void Do(void (I3Module::*f)());

  /**
   * The purpose of this transition is to give this object an opportunity to
   * access all of its parameters so that it will be able to process data.
   * This transition is executed immediately before the first 'Process'
   * transition, thus any parameters that has been set, either by a person
   * or a steering file, will be available to this object before it starts
   * processing data frames. After this transition any changes to these
   * parameters will not be seen by the module until a 'Reconfigure'
   * transition.
   *
   * This transition also gives this object an opportunity to reserve any
   * resources it will need to use during the subsequent 'Process'
   * transitions.
   */
  virtual void Configure();

  // if I should process at all
  virtual bool ShouldDoProcess(I3FramePtr);

  // Process_: check ShouldDoProcess() and thunks to Process()
  void Process_(); 
  
  virtual void Process();

  /**
   * The purpose of this transition is to give this object the opportunity
   * to release any critical resources it may have reserved, as this
   * transition indicates that there will not be another 'Process' transition
   * in the immediately future.
   *
   * Currently unused.
   */
  virtual void Suspend();

  /**
   * The purpose of this transition is to give this object the opportunity to
   * re-access all of its parameters as they may have changed since the
   * previous 'Configure' or 'Reconfigure' transition. In many cases this
   * may simply take the form of repeating whatever was executed at the
   *'Configure' transition.
   *
   * Of course, this transition, like the 'Configure' transition, also gives
   * this object the opportunity to reserve any resources that may have been
   * released during the 'Suspend' transition.
   *
   * Currently unused.
   */
  virtual void Reconfigure();

  /**
   * The purpose of this transition is to solely give this object the
   * opportunity to reserve any resources that may have been released in the
   * proceding 'Suspend' transition. Implicit in this transition is the fact
   * that none of a module's parameters have changed since the last
   * 'Configure' or 'Reconfigure' transition.
   *
   * Currently unused.
   */
  virtual void Resume();

  /**
   * The purpose of this transition is to give this object the opportunity to
   * wind up gracefully. For example a module can use this transition to
   * create a summary of its activities during the execution of the job. Note
   * that after this transition the module is still in existence.
   */
  virtual void Finish();

  /**
   * The purpose of this transition is to give this object the opportunity to
   * clean itself up before it goes out of existence.
   */
  virtual void Dispose();

  /**
   * The purpose of this transition is to give a module the opportunity to
   * clean up before the framework exits due to a 'fatal' error.
   */
  virtual void Abort();

  /**
   * Get the name of a module
   * @return the module name
   */
  const std::string GetName() const { return name_; }

  /**
   * Set the name of a module
   * @param name name of the module
   * @return void
   */
  virtual void SetName(const std::string& name) { name_ = name; }

 protected:

  // void Register(const I3Frame::Stream& when, void(I3Module::*what)(I3FramePtr));
  void Register(const I3Frame::Stream& when, boost::function<void(I3FramePtr)>);

  template <class T>
  void Register(const I3Frame::Stream& when, void (T::*m_fn)(I3FramePtr))
  {
    Register(when, boost::bind(m_fn, static_cast<T*>(this), _1));
  }

  // The following functions are called for the approiate Stop by the
  // default 'Process' implementation.

  /**
   * Called when the frame was generated by a new Physics record.
   */
  virtual void Physics(I3FramePtr frame);

  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoPhysics(I3FramePtr frame);
  
  /**
   * Called when the frame was generated by a new Monitoring record.
   */
  virtual void Monitoring(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoMonitoring(I3FramePtr frame);

  /**
   * Called when the frame was generated by a new TimeCal record.
   */
  virtual void TimeCal(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoTimeCal(I3FramePtr frame);

  /**
   * Called when the frame was generated by a new Geometry record.
   */
  virtual void Geometry(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoGeometry(I3FramePtr frame);

  /**
   * Called when the frame was generated by a new Calibration record.
   */
  virtual void Calibration(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoCalibration(I3FramePtr frame);

  /**
   * Called when the frame was generated by a new DetectorStatus record
   */
  virtual void DetectorStatus(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoDetectorStatus(I3FramePtr frame);

  /**
   * Called when the frame was generated by a new record which is none of
   * the above records.
   */
  virtual void OtherStops(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoOtherStops(I3FramePtr frame);

  /**
   * Requests that the Framework change from the current 'Processing'
   * state to a "Suspened' state.
   *
   * Currently unused.
   */
  void RequestSuspension() const;

  /**
   * If this module buffers frames, when it receives a Finish() it
   * must push all remaining frames and then Flush() to be sure the
   * various stop methods get called downstream.
   */
  virtual void Flush();

  /**
   * Returns the next I3Frame in the default InBox and removes the frame
   * from the box.
   *
   * @return the next I3Frame in the specified InBox.
   */
  I3FramePtr PopFrame();

  // get the next frame w/o removing it from inbox queue.
  // careful you don't process frames multiple times.
  I3FramePtr PeekFrame();

  void AddOutBox(const std::string& name);

  /**
   * Puts the specified I3Frame into the specifed OutBox.
   *
   * @param frame the I3Frame to put into the OutBox.
   * @param name the name associated with the OutBox.
   */
  void PushFrame(I3FramePtr frame, const std::string& name);
  /**
   * Pushes the specified I3Frame onto all outboxes
   * @param frame the frame to push
   */
  void PushFrame(I3FramePtr frame);

  template <typename T>
  void 
  AddParameter(const std::string& parameter,
	       const std::string& description,
	       T defaultValue) const
  {
    configuration_.Add(parameter, description, defaultValue);
  }

  // version w/o default value... must be set in steering file
  void AddParameter(const std::string& name,
		    const std::string& description); 

  template <typename T>
  typename boost::disable_if<boost::is_const<T>, void>::type
  GetParameter(const std::string& name, T& value) const
  {
    try {
      value = configuration_.Get<T>(name);
    } catch (...) {
      log_error("Error in %s module '%s', getting parameter '%s'",
		I3::name_of(typeid(*this)).c_str(), GetName().c_str(), name.c_str());
      throw;
    } 
  }

  /** The context wherein my configuration, outboxes, and so forth can
   be found.
  */
  const I3Context& context_;
  outboxmap_t& outboxes_;
  I3Configuration& configuration_;
  methods_t methods_;

  FrameFifoPtr inbox_;

 private:

  /// Default constructor disabled
  I3Module(); 
  ///  Copy operations make no sense for modules
  I3Module(const I3Module& rhs); 
  ///  Assignment operations make no sense for modules
  I3Module operator=(const I3Module& rhs); 
  
protected:

  virtual void Configure_(); // dispatches to Configure() and sets inbox

 public:

  const I3Context& GetContext() const { return context_; }

  SET_LOGGER("I3Module");

  I3PhysicsUsage ReportUsage();

 private:

  /** module name */
  std::string name_;

  unsigned ncall_;
  double systime_, usertime_;

  // HACK: a set of all boxes added by the module, used for consistency
  // checking.  See Configure_().
  std::set<std::string> added_boxes;

  /// only report usage times if greater than this
  const static double min_report_time_;

};


const I3Context& GetActiveContext();

// Use argument
template <class T>
T& 
GetService(const std::string &where = I3DefaultName<T>::value(), 
	   typename boost::disable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
{
  const I3Context& context = GetActiveContext();

  //Fix to get around the 3.2 compiler
  return context.template Get<T>(where);
}

template <class T>
T 
GetService(const std::string &where = I3DefaultName<typename T::value_type>::value(),
	   typename boost::enable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
{
  const I3Context& context = GetActiveContext();

  //Fix to get around the 3.2 compiler
  return context.template Get<T>(where);
}



#include "icetray/I3Factory.h"
#endif // I3MODULE_H
