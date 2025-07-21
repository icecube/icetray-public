/**
    Copyright  (C) 2004 the icecube collaboration
    SPDX-License-Identifier: BSD-2-Clause
    @version $Id$
    @date $Date$
*/

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <dataclasses/copy_if.h>

#include <icetray/serialization.h>

#include <icetray/IcetrayFwd.h>
#include <icetray/I3FrameObject.h>

#include <icetray/I3Logging.h>


// forward declarations.
template <typename T>
std::string
AsXML(const T& ob);

/**
 * @brief function to get data out of a map-like object.
 */
template <class MapType>
typename MapType::value_type::second_type&
I3MapGet(const typename MapType::key_type& key,
    MapType& themap)
{
  typename MapType::iterator found = themap.find(key);
  if(found == themap.end())
    {
      std::ostringstream error;
      error<<"Couldn't find key "<<key<<" in this map";
      log_fatal("%s",error.str().c_str());
    }
  return found->second;
}

/**
 * @brief function to get data out of a map-like object.
 */
template <class MapType>
const typename MapType::value_type::second_type&
I3MapGet(const typename MapType::key_type& key,
    const MapType& themap)
{
  typename MapType::const_iterator found = themap.find(key);
  if(found == themap.end())
    if(found == themap.end())
      {
        std::ostringstream error;
        error<<"Couldn't find key "<<key<<" in this map";
        log_fatal("%s", error.str().c_str());
      }
  return found->second;
}

#endif
