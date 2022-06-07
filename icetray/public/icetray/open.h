/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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
     *         4 for zstd (.zst)
     * \param mode the mode to use when writing
     */
    void open(boost::iostreams::filtering_ostream&, 
              const std::string& filename,
              int compression_level_ = 0,
              std::ios::openmode mode = std::ios::binary);

  } // namespace dataio
}  //  namespace I3


#endif
