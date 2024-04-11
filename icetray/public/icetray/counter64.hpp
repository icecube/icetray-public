// (C) Copyright Jonathan Turkanis 2005.
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Copyright (C) 2006 Troy D. Straszheim
// This is the counter included with
// boost::iostreams, modified to return a 64-bit size.

#ifndef DATAIO_COUNTER64_HPP_INCLUDED
#define DATAIO_COUNTER64_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <algorithm>  // count.
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/char_traits.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/pipeline.hpp>

// Must come last.
#include <boost/iostreams/detail/config/disable_warnings.hpp> // VC7.1 C4244.

namespace boost {
  namespace iostreams {

    //
    // Template name: basic_counter.
    // Template parameters:
    //      Ch - The character type.
    // Description: Filter which counts lines and characters.
    //
    template<typename Ch>
    class basic_counter64  {
    public:
      typedef Ch char_type;
      struct category
        : dual_use,
          filter_tag,
          multichar_tag,
          optimally_buffered_tag
      { };
      explicit basic_counter64(uint64_t first_char = 0)
        : chars_(first_char)
      { }

      uint64_t characters() const { return chars_; }

      std::streamsize optimal_buffer_size() const { return default_filter_buffer_size; }

      template<typename Source>
      std::streamsize read(Source& src, char_type* s, std::streamsize n)
      {
        std::streamsize result = iostreams::read(src, s, n);
        if (result == -1)
	  return -1;
        chars_ += result;
        return result;
      }

      template<typename Sink>
      std::streamsize write(Sink& snk, const char_type* s, std::streamsize n)
      {
        std::streamsize result = iostreams::write(snk, s, n);
        chars_ += result;
        return result;
      }
    private:
      uint64_t chars_;
    };
    BOOST_IOSTREAMS_PIPABLE(basic_counter64, 1)


    typedef basic_counter64<char>     counter64;

  }
} // End namespaces iostreams, boost.

#include <boost/iostreams/detail/config/enable_warnings.hpp>

#endif // #ifndef BOOST_IOSTREAMS_COUNTER_HPP_INCLUDED
