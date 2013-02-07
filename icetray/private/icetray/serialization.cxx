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

#include <iostream>
#include <map>

#include <icetray/serialization.h>

#if BOOST_VERSION < 104000
#include <boost/archive/impl/archive_pointer_iserializer.ipp>
#include <boost/archive/impl/archive_pointer_oserializer.ipp>

namespace boost {
namespace archive {
  
  template class detail::archive_pointer_iserializer<portable_binary_iarchive> ;
  template class detail::archive_pointer_oserializer<portable_binary_oarchive> ;

} // namespace archive
} // namespace boost
#else
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
namespace archive {
  
  template class detail::archive_serializer_map<portable_binary_iarchive> ;
  template class detail::archive_serializer_map<portable_binary_oarchive> ;

} // namespace archive
} // namespace boost

const char *i3_extended_type_info_key_for_type(const std::type_info & ti,
    const char *guess, i3_extended_type_info_untemplated_intermediate *eti)
{
	typedef std::map<const std::type_info *, std::pair<char *,
	    i3_extended_type_info_untemplated_intermediate *> > keymap_type;
	static keymap_type keys;
	keymap_type::iterator i;
	char *entry;

	i = keys.find(&ti);

	if (i == keys.end()) {
		if (guess == NULL) {
			entry = new char[255];
			entry[0] = '+';
			strncpy(&entry[1], ti.name(), 253);
			entry[254] = 0;
		} else {
			entry = strdup(guess);
		}
		keys.insert(i, keymap_type::value_type(&ti, std::pair<char *,
                    i3_extended_type_info_untemplated_intermediate *>(entry,
		    eti)));
	} else {
		entry = i->second.first;
		if (guess != NULL) {
			if (entry[0] == '+') {
				strncpy(entry, guess, 255);
				entry[254] = 0;
				i->second.second->key_register();
			}
			assert(strcmp(entry, guess) == 0);
		}
	}

	return entry;
}
#endif

