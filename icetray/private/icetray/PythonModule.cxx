//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: ithon.cxx 25598 2006-11-25 02:52:57Z troy $
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>

#include <icetray/I3Module.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Logging.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/OMKey.h>
#include <icetray/PythonModule.h>
#include <boost/python/import.hpp>

namespace bp = boost::python;
template <typename Base>
PythonModule<Base>::PythonModule(const I3Context& ctx) : Base(ctx)
{ 
  log_trace("%s", __PRETTY_FUNCTION__);
}

template <typename Base>
void 
PythonModule<Base>::Configure()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override conf = this->get_override("Configure"))
    conf();
  else
    log_fatal("Python module doesn't implement 'Configure'");
}

template <typename Base>
void 
PythonModule<Base>::Process()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override process = this->get_override("Process"))
    process();
  else
    Base::Process();
}

template <typename Base>
void 
PythonModule<Base>::Finish()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override fin = this->get_override("Finish"))
    fin();
  else
    Base::Finish();
}

template <typename Base>
void
PythonModule<Base>::AddParameter(const std::string& name, 
			   const std::string& description, 
			   const bp::object& value)
{
  Base::AddParameter(name, description, value);
}

template <typename Base>
void
PythonModule<Base>::AddParameter(const std::string& name, 
			   const std::string& description) 
{
  Base::AddParameter(name, description);
}

template <typename Base>
void
PythonModule<Base>::Register(const I3Frame::Stream& s, const boost::python::object& method)
{
  Base::Register(s, method);
}

template <typename Base>
bp::object
PythonModule<Base>::GetParameter(const std::string& name)
{
  bp::object obj;
  Base::GetParameter(name, obj);
  return obj;
}
  
template <typename Base>
void 
PythonModule<Base>::Geometry(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override g = this->get_override("Geometry"))
    g(frame);
  else
    Base::Geometry(frame);
}

template <typename Base>
void 
PythonModule<Base>::Calibration(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override cal = this->get_override("Calibration"))
    cal(frame);
  else
    Base::Calibration(frame);
}

template <typename Base>
void 
PythonModule<Base>::DetectorStatus(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override ds = this->get_override("DetectorStatus"))
    ds(frame);
  else
    Base::DetectorStatus(frame);
}

template <typename Base>
void 
PythonModule<Base>::Physics(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override phys = this->get_override("Physics"))
    phys(frame);
  else
    Base::Physics(frame);
}

template <typename Base>
I3FramePtr
PythonModule<Base>::PopFrame()
{
  return Base::PopFrame();
}

template <typename Base>
void
PythonModule<Base>::PushFrame(I3FramePtr f)
{
  return Base::PushFrame(f);
}

template <typename Base>
void
PythonModule<Base>::PushFrame(I3FramePtr f, const std::string& where)
{
  return Base::PushFrame(f, where);
}

template <typename Base>
void
PythonModule<Base>::RequestSuspension()
{
  return Base::RequestSuspension();
}

template <typename Base>
void
PythonModule<Base>::AddOutBox(const std::string& name)
{
  Base::AddOutBox(name);
}

template class PythonModule<I3Module>;
template class PythonModule<I3ConditionalModule>;



//
//  Custom module creator policy for python modules.  Python class
//  is put in the context by the I3Tray at location 'class'
//
template <class FactoryProductType, class ModuleType>
struct PythonCreate
{
  static 
  shared_ptr<FactoryProductType> 
  Create (const I3Context& c)
  {
    //    try {
      bp::object cls = c.Get<bp::object>("class");
      log_trace("about to cls(c)");
      // use 'ptr' to avoid pass-by-value of context
      bp::object instance = cls(bp::ptr(&c));
      return bp::extract<boost::shared_ptr<FactoryProductType> >(instance);
      //    } catch (...) {
      //      log_fatal("Internal error. Unable to get a boost::python::object named 'class' out of the context.");}
  }
};

namespace {
  typedef void PythonModule;
  I3_REGISTER(I3Module, PythonModule, PythonCreate);
}
//I3_REGISTER(I3Module, PythonModule<I3ConditionalModule>, PythonCreate);
