#include <icetray/name_of.h>

#include <cxxabi.h>
#  include <boost/thread/locks.hpp>
#  include <boost/thread/mutex.hpp>

#include <I3/hash_map.h>
#include <icetray/Utility.h>


namespace icetray
{
  namespace detail
  {    
    class TypenameCache
    {
     private:
      static const unsigned int N_BUCKETS = 1024; 
      static const unsigned int HASH_MASK = N_BUCKETS - 1; // 0x3FF if N_BUCKETS is 1024
      static const char* TYPENAME_NOT_AVAILABLE;
      
      
      struct type_info_ptr_hash
      {
        size_t operator()( const std::type_info* p) const
        {
          return (reinterpret_cast<size_t>(p) >> 6) & HASH_MASK;
        }
      };
      
      
      typedef hash_map<const std::type_info*,
                                  std::string,
                                  type_info_ptr_hash> typename_map_t;
      
      
      boost::mutex mtx_;
      typename_map_t names_;
      
     public:
      /** Default constructor.
       */
      TypenameCache() : names_(N_BUCKETS) {}
      /** Destructor.
       */
      ~TypenameCache(){}
      /** Get a human-readable name for a type.
       * 
       * @param ti The type.
       * @return The name.
       * 
       * @note
       * ...
       * 
       * @todo
       * Use a multiple-reader / single-write model. Does it work, if mixing
       * <em>boost threads</em> and <em>ZThread</em>?
       */
      const std::string& GetName(const std::type_info& ti)
      {
        boost::lock_guard<boost::mutex> g(mtx_);
        
        typename_map_t::iterator iter = names_.find(&ti);
        if(iter != names_.end())
          return iter->second;
        
        const char* mangled = ti.name();        
        if(!mangled)
        {
          names_[&ti] = TYPENAME_NOT_AVAILABLE;
          return names_[&ti];
        }
        
        int status = 0;
        char* demangled = abi::__cxa_demangle(mangled, 0, 0, &status);
        std::string stlfilted;
        try
        {
          if(status)
            stlfilted = mangled;
          else
            stlfilted = demangled ? stlfilt(demangled) : TYPENAME_NOT_AVAILABLE; 
        }
        catch(...)
        {
          if(demangled) free(demangled);
          throw;
        }
        if(demangled) free(demangled);
        
        names_[&ti] = stlfilted;
        return names_[&ti];
      }
       
     private:
      // private copy constructors and assignment
      TypenameCache(const TypenameCache&);
      TypenameCache& operator=(const TypenameCache&);
    };
  
    const char* TypenameCache::TYPENAME_NOT_AVAILABLE = "N/A";
  }
  
  const std::string& name_of(const std::type_info& ti)
  {
    static detail::TypenameCache cache;
    
    return cache.GetName(ti);
  }
}
