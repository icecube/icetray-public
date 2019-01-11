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
// #define BOOST_SPIRIT_DEBUG

#include <icetray/I3Logging.h>

#include <boost/function.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/functions.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/actor.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace boost::spirit;
using namespace phoenix;

namespace dataio
{
  namespace shovel
  {
    struct ignore
    {
      template <typename IteratorT>
      void operator()(IteratorT first, IteratorT last) const
      {
      }
    };
    
    typedef boost::function<void(void)> fn_t;

    struct shovelrc_grammar 
      : public grammar<shovelrc_grammar>
    {
      shovelrc_grammar(map<char, string>& keybdgs)
	: keybdgs_(keybdgs) { }

      mutable map<char, string>& keybdgs_;
      mutable char tmp_key_;

      struct assign_keybinding
      {
	const shovelrc_grammar& g;
	assign_keybinding(const shovelrc_grammar& g_) : g(g_) { }

	template <typename IteratorT>
	void operator()(const IteratorT& start, const IteratorT& end) const
	{
	  string str(start, end);
	  g.keybdgs_[g.tmp_key_] = str;
	  //	  log_info("key %c == %s", g.tmp_key_, str.c_str());
	}
      };

      template <typename ScannerT>
      struct definition
      {
	definition(const shovelrc_grammar& self)
	{
	  comment = ch_p('#') >> (*anychar_p)[ignore()];

	  my_rule =
	    (~ch_p('#'))[assign_a(self.tmp_key_)]
	      >> *space_p >> ch_p('=') >> *space_p 
	      >> (+(alnum_p|ch_p('_')))[assign_keybinding(self)] 
	    ;

	  either = *my_rule >> *comment;
	}
	rule<ScannerT> comment;
	rule<ScannerT> my_rule;
	rule<ScannerT> either;

	rule<ScannerT> const&
	start() const { return either; }
      };

    };

    bool parse_rcfile(const std::string& filename,
		      map<char, string>& keybindings)
    {
      ifstream ifs(filename.c_str());
      shovelrc_grammar g(keybindings);
      string str;
      unsigned line_no = 1;
      while (getline(ifs, str))
	{
#if BOOST_VERSION < 013600
	  if (!parse(str.c_str(), g, space_p).full)
	    log_fatal("Failure parsing %s at line %u", 
		      filename.c_str(), line_no);
#else
#warning fixme
#endif
	  line_no++;
	}
      return true;
    }

  } // shovel
} // dataio

