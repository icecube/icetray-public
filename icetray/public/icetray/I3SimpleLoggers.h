/**
 *    $Id$
 *
 *    Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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

#ifndef ICETRAY_I3SIMPLELOGGERS_H_INCLUDED
#define ICETRAY_I3SIMPLELOGGERS_H_INCLUDED

#include <icetray/I3Logging.h>

class I3NullLogger : public I3Logger {
public:
	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message) {};
}; 

class I3PrintfLogger : public I3Logger {
public:
	I3PrintfLogger(I3LogLevel default_level = I3DefaultLogLevel);
	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message);

	bool TrimFileNames;
private:
	bool tty_;
};

#endif //ifndef ICETRAY_I3SIMPLELOGGERS_H_INCLUDED
