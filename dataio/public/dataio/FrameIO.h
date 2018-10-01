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
#ifndef DATAIO_FRAMEIO_H_INCLUDED
#define DATAIO_FRAMEIO_H_INCLUDED

#include <icetray/serialization.h>

#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <boost/regex.hpp>

#include <map>
#include <set>
#include <string>

using namespace boost::archive;


namespace I3 {
  namespace dataio {

    void open(boost::iostreams::filtering_istream&, const std::string& filename);
    void open(boost::iostreams::filtering_ostream&, 
	      const std::string& filename, 
	      unsigned gzip_compression_level_=6, 
	      std::ios::openmode = std::ios::binary);

    template <class OArchive, class OStream>
    void 
    save(const I3Frame& frame, 
	 OStream& ofs, 
	 const std::vector<std::string> &skip = std::vector<std::string>());

    template <class IArchive, class IStream>
    I3FramePtr
    load(IStream& ifs,
	 const std::vector<std::string>& skip = std::vector<std::string>(),
	 bool skip_unregistered = false);

    std::string
    buf2xml(const std::string& buf, const std::string& key);

  } // namespace dataio
}  //  namespace I3

#endif
