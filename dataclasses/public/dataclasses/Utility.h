/**
    copyright  (C) 2004
    the icecube collaboration
    @version $Id$
    @date $Date$
*/

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#if defined(__APPLE__) && defined(__CINT__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1085)
// Workaround for CINT problems with stdint.h. Affects OS X >= 10.9; see
// https://root.cern.ch/phpBB3/viewtopic.php?f=5&t=17360
typedef char __signed;
#include <sys/_types/_int8_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <stdint.h> //int64_t, etc
#endif

#include <dataclasses/copy_if.h>

#ifndef __CINT__
#include <icetray/serialization.h>
#endif

#include <icetray/IcetrayFwd.h>
#include <icetray/I3FrameObject.h>

#include <icetray/I3Logging.h>


// forward declarations.  
template <typename T>
std::string 
AsXML(const T& ob);

// hide Get from ROOT since Rene doesn't like templates
#ifndef __CINT__

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

#endif 
