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
#ifndef ICETRAY_OPEN_H_INCLUDED
#define ICETRAY_OPEN_H_INCLUDED

#include <string>
#include <boost/iostreams/filtering_stream.hpp>

namespace I3 {
  namespace dataio {

    void open(boost::iostreams::filtering_istream&, const std::string& filename);

    /**
     * Open an output file using compression if indicated by an extension on the
     * file name.
     * \param filename the path to which the file must be written
     * \param compression_level_ the compression level to use if writing a 
     *        compressed format. The default value of zero will be interpreted 
     *        in a format dependent way, being replaced by:
     *         6 for gzip (.gz)
     *         6 for bzip2 (.bz2)
     * \param mode the mode to use when writing
     */
    void open(boost::iostreams::filtering_ostream&, 
              const std::string& filename,
              int compression_level_ = 0,
              std::ios::openmode mode = std::ios::binary);

  } // namespace dataio
}  //  namespace I3


#endif
