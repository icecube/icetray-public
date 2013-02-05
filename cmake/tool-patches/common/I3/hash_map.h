#ifndef I3_HASH_STRING_H_INCLUDED
#define I3_HASH_STRING_H_INCLUDED

#include <string>

#if __cplusplus < 201103L
#define USING_GCC_EXT_HASH_MAP 1
#include <ext/hash_map>

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

//No template aliases in C++03, so do this the risky way:
#define hash_map __gnu_cxx::hash_map
using __gnu_cxx::hash;

#else
#include <unordered_map>

struct djb2_hash{
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

template<typename T>
struct i3hash : public std::hash<T>{};
template<>
struct i3hash<std::string> : djb2_hash{};

template<typename K, typename V, typename Hash=i3hash<K>>
using hash_map = std::unordered_map<K,V,Hash>;

template<typename T>
using hash = i3hash<T>;

#endif // __cplusplus < 201103L

#endif
