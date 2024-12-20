/**
 *  $Id$
 *
 *  Copyright (C) 2007, 2008 Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Copyright (C) 2007, 2008 the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  SPDX-License-Identifier: BSD-2-Clause
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
#include <icetray/python/gil_holder.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/python/object.hpp>
#include <boost/python/extract.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

class I3Configuration;
class I3Context;
class I3FrameMixer;

/**
 * This class defines the interface which should be implemented by all
 * modules in IceTray.
 *
 * @version $Id$
 */
class I3Module;
I3_POINTER_TYPEDEFS(I3Module);

class I3Module
{
  typedef std::map<std::string, std::pair<FrameFifoPtr, I3ModulePtr> > outboxmap_t;

  typedef std::map<I3Frame::Stream, boost::function<void(I3FramePtr)> > methods_t;

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
   * processing data frames.
   *
   * This transition also gives this object an opportunity to reserve any
   * resources it will need to use during the subsequent 'Process'
   * transitions.
   */
  virtual void Configure();

  // internal method: thunks to Configure()
  virtual void Configure_();

  // if I should process at all
  virtual bool ShouldDoProcess(I3FramePtr);

  // Process_: check ShouldDoProcess() and thunks to Process()
  void Process_();

  virtual void Process();

  /**
   * The purpose of this transition is to give this object the opportunity to
   * wind up gracefully. For example a module can use this transition to
   * create a summary of its activities during the execution of the job. Note
   * that after this transition the module is still in existence.
   */
  virtual void Finish();

  /**
   * Get the name of a module
   * @return the module name
   */
  const std::string GetName() const { return name_; }

  /**
   * Set the name of a module
   * @param name name of the module
   */
  virtual void SetName(const std::string& name) { name_ = name; }

 protected:

  void Register(const I3Frame::Stream& when, boost::function<void(I3FramePtr)>);

  template <class T>
  void Register(const I3Frame::Stream& when, void (T::*m_fn)(I3FramePtr))
  {
    Register(when, boost::bind(m_fn, static_cast<T*>(this), _1));
  }

  // The following functions are called for the appropriate Stop by the
  // default 'Process' implementation.

  /**
   * Called when the frame was generated by a new DAQ record.
   */
  virtual void DAQ(I3FramePtr frame);

  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the DAQ(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoDAQ(I3FramePtr frame);


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
   * Called when the frame was generated by a new Simulation record
   */
  virtual void Simulation(I3FramePtr frame);
  /**
   *  ShouldDo* will be reimplemented. Probably better if you don't implement.
   *   Instead, just put any predicates like this in the Physics(), etc
   *    and push the frame out early if not interested.
   */
  virtual bool ShouldDoSimulation(I3FramePtr frame);

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
   * Puts the specified I3Frame into the specified OutBox.
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
	       T defaultValue)
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
    log_debug("I've entered GetParameter and am trying it first with Get<T>: %s", name.c_str());
    try {
      value = configuration_.Get<T>(name);
    } catch (...) {
      // This is a bit fiddly, because if we can get the parameter as a string, then get an object
      // of the correct type from the context using that as a key, we want to just suppress the
      // original error/exception, but if the fall-back fails, we want to report the original error.
      // There may or may not be a python error involved, so if so, we need to capture it from the
      // global state so that in the meantime the python interpreter doesn't get confused and
      // believe that it has been emitted from any other python code which might be invoked.
      boost::python::detail::restore_exceptions pyError;
      log_debug_stream("...which failed the first time, so trying it again with Get<string>: " << name);
      // Bail if the value is not a string, or that string is not a key in the context
      // special case for Get<shared_ptr<T>>, which never throws
      boost::python::extract<std::string> get_context_name(configuration_.Get(name));
      if (!get_context_name.check() || !(is_shared_ptr<T>::value || context_.Has(get_context_name()))) {
        throw;
      }
      try {
        log_debug_stream("Attempting a Get<T> on context name " << get_context_name());
        value = context_.Get<T>(get_context_name());
      } catch (...) {
        log_error("Error in %s module '%s', getting parameter '%s'",
                  I3::name_of(typeid(*this)).c_str(), GetName().c_str(),
                  name.c_str());
        pyError.set_raised_exception();
        throw;
      }
      pyError.clear();
    }
  }

  template<typename T>
  T& GetService(const std::string &where = I3DefaultName<T>::value(),
     typename boost::disable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
  {
    return context_.Get<T>(where);
  }

  template<typename T>
  T GetService(const std::string &where = I3DefaultName<typename T::element_type>::value(),
     typename boost::enable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
  {
    return context_.Get<T>(where);
  }

  /** The context wherein my configuration, outboxes, and so forth can
   be found.
  */
  const I3Context& context_;
  outboxmap_t outboxes_;
  I3Configuration configuration_;
  methods_t methods_;

  FrameFifoPtr inbox_;

 private:

  /// Default constructor disabled
  I3Module();
  ///  Copy operations make no sense for modules
  I3Module(const I3Module& rhs);
  ///  Assignment operations make no sense for modules
  I3Module operator=(const I3Module& rhs);

 public:

  const I3Context& GetContext() const { return context_; }
  I3Configuration& GetConfiguration() { return configuration_; }
  const I3Configuration& GetConfiguration() const { return configuration_; }
  ///Connect one of this module's outboxes to another module
  ///\param outBoxName the outbox to be connected
  ///\param targetModule the module to which the outbox will connect
  void ConnectOutBox(const std::string& outBoxName, I3ModulePtr targetModule);
  ///Check whether all of this module's outboxes connect to other modules
  bool AllOutBoxesConnected() const;
  ///Test whether this module has a particular outbox
  ///\param outBoxName the name for which to check
  bool HasOutBox(const std::string& outBoxName) const;
  ///Test whether this module has a valid inbox from which it can receive frames
  bool HasInBox() const;

  SET_LOGGER("I3Module");

  I3PhysicsUsage ReportUsage();

 private:

  /** module name */
  std::string name_;

  unsigned nphyscall_, ndaqcall_;
  double systime_, usertime_;

  // cache of previous metadata frames (per-outbox)
  std::map<std::string, boost::shared_ptr<I3FrameMixer> > cachemap_;
  void SyncCache(std::string outbox, I3FramePtr frame);

  /// only report usage times if greater than this
  const static double min_report_time_;

};

#include "icetray/I3Factory.h"
#endif // I3MODULE_H
