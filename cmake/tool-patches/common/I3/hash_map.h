#ifndef I3_HASH_STRING_H_INCLUDED
#define I3_HASH_STRING_H_INCLUDED

#include <string>

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

#if __cplusplus < 201103L
#define USING_GCC_EXT_HASH_MAP 1
#include <ext/hash_map>

namespace __gnu_cxx
{
  template <> struct hash< std::string > : public djb2_hash{};
}

using __gnu_cxx::hash_map;
using __gnu_cxx::hash;

#else
#include <unordered_map>

template<typename T>
struct i3hash : public std::hash<T>{};
template<>
struct i3hash<std::string> : djb2_hash{};

template<typename K, typename V, typename Hash=i3hash<K>,
         typename Pred = std::equal_to<K>,
         typename Alloc = std::allocator< std::pair<const K,V> > >
using hash_map = std::unordered_map<K,V,Hash,Pred,Alloc>;

template<typename T>
using hash = i3hash<T>;

#endif // __cplusplus < 201103L

#include <serialization/collections_save_imp.hpp>
#include <serialization/collections_load_imp.hpp>
#include <serialization/split_free.hpp>

namespace icecube {
  namespace serialization {
    
    template<class Archive, class Type, class Key, class Hash, class Equal, class Allocator >
    inline void save(Archive & ar,
                     const hash_map<Key, Type, Hash, Equal, Allocator> &t,
                     const unsigned int /* file_version */
                     ){
      icecube::serialization::stl::save_collection<
        Archive,
        hash_map<Key, Type, Hash, Equal, Allocator>
      >(ar, t);
    }
    
    template<class Archive, class Type, class Key, class Hash, class Equal, class Allocator >
    inline void load(Archive & ar,
                     hash_map<Key, Type, Hash, Equal, Allocator> &t,
                     const unsigned int /* file_version */
                     ){
      icecube::serialization::stl::load_collection<
        Archive,
        hash_map<Key, Type, Hash, Equal, Allocator>,
        icecube::serialization::stl::archive_input_map<
          Archive, hash_map<Key, Type, Hash, Equal, Allocator> >,
          icecube::serialization::stl::no_reserve_imp<hash_map<
            Key, Type, Hash, Equal, Allocator
          >
        >
      >(ar, t);
    }
    
    // split non-intrusive serialization function member into separate
    // non intrusive save/load member functions
    template<class Archive, class Type, class Key, class Hash, class Equal, class Allocator >
    inline void serialize(Archive & ar,
                          hash_map<Key, Type, Hash, Equal, Allocator> &t,
                          const unsigned int file_version
                          ){
      icecube::serialization::split_free(ar, t, file_version);
    }
  }
}

#endif
