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
#include <dataio/I3Writer.h>
#include <icetray/open.h>

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3Writer);

I3Writer::I3Writer(const I3Context& ctx) 
  : I3WriterBase(ctx)
{ }

I3Writer::~I3Writer() { }

void
I3Writer::Configure_()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  I3ConditionalModule::Configure_();
  current_filename_ = file_stager_->GetWriteablePath(path_);
  dataio::open(filterstream_, *current_filename_, gzip_compression_level_);
}

void
I3Writer::Finish()
{
  filterstream_.reset();
  I3WriterBase::Finish();
}
