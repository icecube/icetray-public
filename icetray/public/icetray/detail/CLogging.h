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
#ifndef ICETRAY_DETAIL_CLOGGING_H_INCLUDED
#define ICETRAY_DETAIL_CLOGGING_H_INCLUDED

//
// c-linkage logging functions.  
//

#ifdef __cplusplus
extern "C" 
{
#endif
  void i3_clog_trace(const char* logger, const char* msg);
  void i3_clog_debug(const char* logger, const char* msg);
  void i3_clog_info(const char* logger, const char* msg);
  void i3_clog_warn(const char* logger, const char* msg);
  void i3_clog_error(const char* logger, const char* msg);
  void i3_clog_fatal(const char* logger, const char* msg);
#ifdef __cplusplus
}
#endif
#endif //ifndef ICETRAY_DETAIL_CLOGGING_H_INCLUDED
