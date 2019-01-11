#include <icetray/name_of.h>

#include <cxxabi.h>
#include <string>
#include <I3/hash_map.h>
#include <icetray/Singleton.h>
#include <icetray/Utility.h>

namespace icetray {

  namespace detail {
    const unsigned nbuckets = 1024; 
    const unsigned hash_mask = nbuckets-1; // 0x3FF if nbuckets is 1024

    struct type_info_ptr_hash
    {
      size_t operator()( const std::type_info* p) const
      {
	return (reinterpret_cast<size_t>(p) >> 6) & hash_mask;
      }
    };
  }

  const std::string& name_of(const std::type_info &ti)
  {
    typedef __gnu_cxx::hash_map<const std::type_info*, std::string, detail::type_info_ptr_hash> 
      typename_map_t;

    const static std::string typename_notavailable = "N/A";

    static typename_map_t names(detail::nbuckets);

    typename_map_t::iterator iter = names.find(&ti);
    if (iter != names.end())
      return iter->second;

    const char* mangled = ti.name();

    if (!mangled)
      {
	names[&ti] = typename_notavailable;
	return names[&ti];
      }

    int status;

    char* demangled = abi::__cxa_demangle(mangled, 0, 0, &status);
    std::string stlfilted;
    if (status != 0)
      stlfilted = mangled;
    else
      stlfilted = demangled ? stlfilt(demangled) : typename_notavailable; 
      
    free(demangled);
    names[&ti] = stlfilted;
    return names[&ti];
  }
}
