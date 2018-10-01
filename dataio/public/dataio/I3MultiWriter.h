/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#ifndef DATAIO_I3MULTIWRITER_H_INCLUDED
#define DATAIO_I3MULTIWRITER_H_INCLUDED

#include <dataio/I3WriterBase.h>
#include <sstream>

class I3MultiWriter : public I3WriterBase<I3MultiWriter>
{
  I3MultiWriter();
  I3MultiWriter(const I3MultiWriter&);
  
  uint64_t size_limit_;
  unsigned    file_counter_;
  void NewFile();
  
  std::string current_path();

public:

  I3MultiWriter(const I3Context& ctx);

  virtual ~I3MultiWriter();

  void Configure_();
  void Flush();
  void Finish_();

};

#endif
