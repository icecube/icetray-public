#ifndef I3_HASH_STRING_H_INCLUDED
#define I3_HASH_STRING_H_INCLUDED

#include <string>

#if 1 //GCC_VERSION < 40300
#include <ext/hash_map>
#else
#include <unordered_map>
#endif

namespace __gnu_cxx
{
  template <> struct hash< std::string > 
  {
    size_t operator()( const std::string& x ) const
    {
      const char* str = x.c_str();
      // the magic djb2 hash
      size_t hash = 5381;
      int c;
      while ((c = *str++))
	hash = ((hash << 5) + hash) + c; // hash * 33 + c
      return hash;
    }
  };
}


#endif
