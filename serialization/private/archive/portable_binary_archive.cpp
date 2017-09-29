#include <limits>

#include <archive/portable_binary_archive.hpp>

#include <archive/impl/archive_serializer_map.ipp>

namespace icecube {
namespace archive {

namespace detail {
    // make a space-efficient integer save/load
    template<typename T>
    struct variable_int
    {
        const T ff = std::numeric_limits<T>::max();
        void save(portable_binary_oarchive& pba, uint64_t t)
        {
            if (t >= ff) {
                pba << ff;
                pba << t;
            } else {
                const T x = t;
                pba << x;
            }
        }
        void load(portable_binary_iarchive& pba, uint64_t& t)
        {
            T x = 0;
            pba >> x;
            if (x == ff) {
                pba >> t;
            } else {
                t = x;
            }
        }
    };
}

void portable_binary_oarchive::save_override(const version_type& t, I3_PFTO int) {
    detail::variable_int<uint8_t> v;
    v.save(*this, t);
}

void portable_binary_oarchive::save_override(const serialization::collection_size_type& t, I3_PFTO int) {
    detail::variable_int<uint32_t> v;
    v.save(*this, t);
}

void portable_binary_oarchive::save_override(const std::string& s, I3_PFTO int) {
    detail::variable_int<uint32_t> v;
    v.save(*this, s.size());
    save_binary(&(s[0]), s.size());
}

#ifndef BOOST_NO_STD_WSTRING
void portable_binary_oarchive::save_override(const std::wstring& s, I3_PFTO int) {
    detail::variable_int<uint32_t> v;
    v.save(*this, s.size());
    //individual wide characters may need byte swapping
    for(wchar_t c : s)
        save(c);
}
#endif

void portable_binary_oarchive::save_override(const class_name_type& t, I3_PFTO int) {
    std::string s(t.t);
    detail::variable_int<uint32_t> v;
    v.save(*this, s.size());
    save_binary(&(s[0]), s.size());
}

void portable_binary_iarchive::load_override(version_type& t, int) {
    detail::variable_int<uint8_t> v;
    uint64_t x;
    v.load(*this, x);
    t = (version_type)x;
}

void portable_binary_iarchive::load_override(serialization::collection_size_type& t, int) {
    detail::variable_int<uint32_t> v;
    uint64_t x;
    v.load(*this, x);
    t = (serialization::collection_size_type)x;
}

void portable_binary_iarchive::load_override(std::string& s, I3_PFTO int) {
    uint64_t l;
    detail::variable_int<uint32_t> v;
    v.load(*this, l);
    s.resize(l);
    load_binary(&(s[0]), l);
}

#ifndef BOOST_NO_STD_WSTRING
void portable_binary_iarchive::load_override(std::wstring& s, I3_PFTO int) {
    uint64_t l;
    detail::variable_int<uint32_t> v;
    v.load(*this, l);
    s.resize(l);
    for(size_t i=0; i<l; i++)
        load(s[i]);
}
#endif

        
template class detail::archive_serializer_map<portable_binary_iarchive> ;
template class detail::archive_serializer_map<portable_binary_oarchive> ;
        
} // namespace archive
} // namespace icecube
