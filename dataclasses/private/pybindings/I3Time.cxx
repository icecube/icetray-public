//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//

// this pragma has to go before any functions or whatever are defined.
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <vector>

#include <dataclasses/I3Time.h>
#include <Python.h>
#include <datetime.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

#define HAVE_PYDATETIME_API

std::string repr(I3Time t){
  std::stringstream out;
  out <<  "I3Time("<< std::setw(4) << t.GetUTCYear() << "," <<  std::setw(18) <<t.GetUTCDaqTime() << "L)";
  return out.str();
}

#ifdef HAVE_PYDATETIME_API
boost::python::object GetDateTime(const I3Time& t)
{

  PyObject* obj;
  if (t.IsLeapSecond()){
    //python's datetime won't allow a leap second to be input from python
    //so set to last microsecond of the day: 23:59:59.999999
    obj = PyDateTime_FromDateAndTime(t.GetUTCYear(), 
				     t.GetUTCMonth(), 
				     t.GetUTCDayOfMonth(),
				     23,
				     59,
				     59,
				     999999
				     );
  }else{
    obj = PyDateTime_FromDateAndTime(t.GetUTCYear(), 
				     t.GetUTCMonth(), 
				     t.GetUTCDayOfMonth(),
				     t.GetModJulianSec()/3600,
				     t.GetModJulianSec()%3600/60,
				     t.GetModJulianSec()%60,
				     t.GetModJulianMicroSec()//round down to nearest microsecond
				     );
  }
  handle<> h(obj);
  boost::python::object o(h);
  return o;
}

I3Time GetI3Time(const boost::python::object& datetime_obj)
{
  I3Time t;
  PyObject* datetime = datetime_obj.ptr();

  //datetime is a subclass of date so check for that before date
  if (PyDateTime_Check(datetime))
    {
      t.SetUTCCalDate(PyDateTime_GET_YEAR(datetime),
		      PyDateTime_GET_MONTH(datetime),
		      PyDateTime_GET_DAY(datetime),
		      PyDateTime_DATE_GET_HOUR(datetime),
		      PyDateTime_DATE_GET_MINUTE(datetime),
		      PyDateTime_DATE_GET_SECOND(datetime),
		      PyDateTime_DATE_GET_MICROSECOND(datetime)*1e3
		      );
    }
  else if (PyDate_Check(datetime))
    {
      t.SetUTCCalDate(PyDateTime_GET_YEAR(datetime),
		      PyDateTime_GET_MONTH(datetime),
		      PyDateTime_GET_DAY(datetime),
		      0,0,0,0);
    }
  else
    {
      PyErr_SetString(PyExc_TypeError, "Argument for GetI3Time must be of type datetime.datetime or datetime.date");
      throw_error_already_set();
    }
  return t;
}
#endif


void set_unix_time_default(I3Time& t, time_t ut){
  t.SetUnixTime(ut);
}

int32_t  (*fx1)(double) = &I3Time::DayOfYear;
int32_t  (*fx2)(int64_t) = &I3Time::DayOfYear;
double   (*fx3)(int) = &I3Time::modjulianday;
double   (*fx4)(int, int64_t) = &I3Time::modjulianday;
double   (*fx5)(int) = &I3Time::julianday;
double   (*fx6)(int, int64_t) = &I3Time::modjulianday;

// MonthToString, WeekdayToString are better done in pythonia


I3Time I3CalendarDate(int year,
		      int month,
		      int day,
		      int hour,
		      int minute,
		      int sec,
		      double ns)
{
  I3Time t;
  t.SetUTCCalDate(year,month,day,hour,minute,sec,ns);
  return t;
}

char I3DalendarDate_docstring [] =
  "Alternate constructor for I3Time. "
  "Generates an I3Time object with a specified calendar date.\n"
  "\n"
  "Equivelent to:\n"
  "\n"
  "    t = I3Time()\n"
  "    t.SetUTCCalDate(year,month,day,hour,minute,sec,nanosecond)\n"
  "\n"
  "Example\n"
  "-------\n"
  "    print dataclasses.I3CalendarDate(2012,6,30,23,59,60,0.5e9)\n"
  "    2012-06-30 23:59:60.500,000,000,0 UTC\n";

void register_I3Time()
{
#ifdef HAVE_PYDATETIME_API
  PyDateTime_IMPORT;

  def("make_I3Time",&GetI3Time);
#endif
  def("day_of_year",fx1,"I3Time::DayOfYear(double modjulianday)");
  def("day_of_year",fx2,"I3Time::DayOfYear(int64_t daqTime)");
  def("modjulianday", fx3,"I3Time::modjulianday(int year)");
  def("modjulianday", fx4,"I3Time::modjulianday(int year, int64_t daqTime)");
  def("julianday", fx5,"I3Time::julianday(int year)");
  def("julianday", fx6,"I3Time::julianday(int year, int64_t daqTime)");
  def("year_of",&I3Time::yearOf,"I3Time::yearOf(double modjulianday)");
  def("max_daq_time",I3TimeUtils::max_DAQ_time);
  def("leap_second_on_mjd",I3TimeUtils::leap_sec_on_mjd);
  def("I3CalendarDate",I3CalendarDate,
      args("year","month","day","hour","minute","sec","nanosecond"),
      I3DalendarDate_docstring);

  scope i3time_scope = class_<I3Time, bases<I3FrameObject>, 
    boost::shared_ptr<I3Time> >("I3Time")
    .def(init<int32_t,int64_t>())
    .def(init<const I3Time&>())
    .def(init<double>())
    #define DEFS (SetModJulianTime)(SetModJulianTimeDouble)(SetUTCCalDate)(SetUnixTime)(SetDaqTime)
    BOOST_PP_SEQ_FOR_EACH(WRAP_DEF_RECASE, I3Time, DEFS)
    #undef  DEFS
    .add_property("is_leap_second",&I3Time::IsLeapSecond)
#ifdef HAVE_PYDATETIME_API
    .add_property("date_time", &GetDateTime)
#endif
#define RO_PROPS (ModJulianDay)(ModJulianSec)(ModJulianNanoSec)(ModJulianMicroSec)(ModJulianDayDouble)(UnixTime) \
      (UTCYear)(UTCDaqTime)(UTCMonth)(UTCWeekday)(UTCDayOfMonth)(UTCSec)(UTCNanoSec)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Time, RO_PROPS)
    #undef  RO_PROPS
    .def("__str__",&stream_to_string<I3Time>)
    .def("__repr__",&repr)
    .def("get_utc_string",&I3Time::GetUTCString)
    .def(self != self)
    .def(self == self)
    .def(self-self)
    .def(self-double())
    .def(self+double())
    .def(self<self)
    .def(self>self)
    .def(self<=self)
    .def(self>=self)
    .def(dataclass_suite<I3Time>())
    ;


  register_pointer_conversions<I3Time>();
  
  enum_<I3Time::Month>("Month")
    .value("Jan", I3Time::Jan)
    .value("Feb", I3Time::Feb)
    .value("Mar", I3Time::Mar)
    .value("Apr", I3Time::Apr)
    .value("May", I3Time::May)
    .value("Jun", I3Time::Jun)
    .value("Jul", I3Time::Jul)
    .value("Aug", I3Time::Aug)
    .value("Sep", I3Time::Sep)
    .value("Oct", I3Time::Oct)
    .value("Nov", I3Time::Nov)
    .value("Dec", I3Time::Dec)
    .value("BadMonth", I3Time::BadMonth)
    .export_values()
    ;

  enum_<I3Time::Weekday>("Weekday")
    .value("Sunday", I3Time::Sunday)
    .value("Monday", I3Time::Monday)
    .value("Tuesday", I3Time::Tuesday)
    .value("Wednesday", I3Time::Wednesday)
    .value("Thursday", I3Time::Thursday)
    .value("Friday", I3Time::Friday)
    .value("Saturday", I3Time::Saturday)
    .value("BadWeekday", I3Time::BadWeekday)
    .export_values()
    ;
}
