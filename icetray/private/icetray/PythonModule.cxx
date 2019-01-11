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
#include <icetray/I3Logging.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/OMKey.h>
#include <icetray/PythonModule.h>
#include <boost/python/import.hpp>

namespace bp = boost::python;

PythonModule::PythonModule(const I3Context& ctx) : I3Module(ctx)
{ 
  log_trace("%s", __PRETTY_FUNCTION__);
}

void 
PythonModule::Configure()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override conf = this->get_override("Configure"))
    conf();
  else
    log_fatal("Python module doesn't implement 'Configure'");
}

void 
PythonModule::Process()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override process = this->get_override("Process"))
    process();
  else
    I3Module::Process();
}

void 
PythonModule::Finish()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override fin = this->get_override("Finish"))
    fin();
  else
    I3Module::Finish();
}

void
PythonModule::AddParameter(const std::string& name, 
			   const std::string& description, 
			   const bp::object& value)
{
  I3Module::AddParameter(name, description, value);
}

void
PythonModule::AddParameter(const std::string& name, 
			   const std::string& description) 
{
  I3Module::AddParameter(name, description);
}

void
PythonModule::Register(const I3Frame::Stream& s, const boost::python::object& method)
{
  I3Module::Register(s, method);
}

bp::object
PythonModule::GetParameter(const std::string& name)
{
  bp::object obj;
  I3Module::GetParameter(name, obj);
  return obj;
}
  
void 
PythonModule::Geometry(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override g = this->get_override("Geometry"))
    g(frame);
  else
    I3Module::Geometry(frame);
}

void 
PythonModule::Calibration(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override cal = this->get_override("Calibration"))
    cal(frame);
  else
    I3Module::Calibration(frame);
}

void 
PythonModule::DetectorStatus(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override ds = this->get_override("DetectorStatus"))
    ds(frame);
  else
    I3Module::DetectorStatus(frame);
}

void 
PythonModule::Physics(I3FramePtr frame)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  if (bp::override phys = this->get_override("Physics"))
    phys(frame);
  else
    I3Module::Physics(frame);
}

I3FramePtr
PythonModule::PopFrame()
{
  return I3Module::PopFrame();
}

void
PythonModule::PushFrame(I3FramePtr f)
{
  return I3Module::PushFrame(f);
}

void
PythonModule::PushFrame(I3FramePtr f, const std::string& where)
{
  return I3Module::PushFrame(f, where);
}

void
PythonModule::AddOutBox(const std::string& name)
{
  I3Module::AddOutBox(name);
}

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
    try {
      bp::object cls = c.Get<bp::object>("class");
      log_trace("about to cls(c)");
      // use 'ptr' to avoid pass-by-value of context
      bp::object instance = cls(bp::ptr(&c));
      return bp::extract<boost::shared_ptr<FactoryProductType> >(instance);
    } catch (...) {
      log_warn("Unable to get a boost::python::object named 'class' out of the context... this will be a braindead pythonmodule");
      return boost::shared_ptr<FactoryProductType>(new PythonModule(c));
    }
  }
};

I3_REGISTER(I3Module, PythonModule, PythonCreate);
