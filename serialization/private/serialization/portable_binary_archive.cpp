#include <archive/portable_binary_archive.hpp>

#include <archive/impl/archive_serializer_map.ipp>

namespace icecube {
    namespace archive {
        
        template class detail::archive_serializer_map<portable_binary_iarchive> ;
        template class detail::archive_serializer_map<portable_binary_oarchive> ;
        
    } // namespace archive
} // namespace icecube
