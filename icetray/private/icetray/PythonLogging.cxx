/**
 *    $Id$
 *
 *    Copyright (C) 2008   Troy D. Straszheim  <troy@icecube.umd.edu>
 *    and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *    
 *    This file is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef I3_PYTHON_LOGGING

#include <stdexcept>
#include <stdio.h>

#include <icetray/I3Logging.h>
#include <icetray/Singleton.h>
#include <icetray/PythonInterpreter.h>
#include <boost/python.hpp>

namespace bp = boost::python;

namespace I3Logging 
{
  struct python_logger 
  {
    boost::python::object logger_obj, log, main_module, main_namespace;

    python_logger() 
    {
      const I3::InitializeInterpreter& ii 
	= I3::Singleton<I3::InitializeInterpreter>::get_mutable_instance().init();

      main_module = bp::import("__main__");
      main_namespace = main_module.attr("__dict__");

      try {
	bp::exec("\
import logging\n\
logging.addLevelName(5, 'TRACE')\n\
class CppLogger(logging.Logger):\n\
  def __init__(self, name, level=logging.NOTSET):\n\
    logging.Logger.__init__(self, name, level)\n\
  def makeRecord(self, name, level, fn, lno, msg, args, exc_info, func=None, extra=None):\n\
    rv = logging.LogRecord(name, level, extra['cpp_filename'], extra['cpp_lineno'], msg, args, exc_info, func)\n\
    return rv\n\
hdlr = logging.StreamHandler()\n\
fmt = logging.Formatter('%(filename)s:%(lineno)d %(levelname)s %(message)s')\n\
hdlr.setFormatter(fmt)\n\
cpplogger = CppLogger(logging.INFO)\n\
cpplogger.addHandler(hdlr)\n\
def cpp_log(level, msg, file, lineno):\n\
  cpplogger.log(level, msg, extra={ 'cpp_filename' : file, 'cpp_lineno' : lineno})\n\
"
	       ,  main_namespace, main_namespace);

      log = main_module.attr("cpp_log");
      assert(PyFunction_Check(log.ptr()));
      printf("Checks passed\n");

      } catch (const bp::error_already_set& e) {
	PyErr_Print();
	printf("RRORR RROR");
      }
      printf("DONE EVAL\n");
    }

    //l.setLoggerClass(CppLogger)\n
  };

  const static python_logger& pylog = I3::Singleton<python_logger>::get_mutable_instance();

  bool logger::log(int level, const char* file, int line, const char* fmt, ...)
  {
    va_list ap;
    unsigned int ret = 0;
  
    va_start(ap, fmt);
    ret = vsnprintf(buf, bufsize, fmt, ap);
    va_end(ap);

    //printf("%d %s:%d %s\n", level, file, line, buf);
    I3::Singleton<python_logger>::get_mutable_instance().log(level, buf, file, line);

    if (level == FATAL)
      throw std::runtime_error(buf);

    return true;
  }
}

I3Logging::logger& get_logger()
{ 
  static I3Logging::logger thelogger;
  return thelogger; 
}


#endif
