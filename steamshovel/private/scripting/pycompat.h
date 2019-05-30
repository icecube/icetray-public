#ifndef I3_SHOVEL_PYCOMPAT_H
#define I3_SHOVEL_PYCOMPAT_H

// This file exists to pre-declare specializations for shared_ptr deleters that operate
// on objects that may be boost::python::wrappers around Python objects.
// For such objects the GIL must be held when destructors run.
// Gory details are given in pycompat.cpp
// Both this file and pycompat.cpp must be updated if new wrapper types are added to shovelart.

// ensure python version is defined
#include <boost/python/detail/prefix.hpp>

#if PY_VERSION_HEX >= 0x02070300

#include <boost/python/handle.hpp>
#include <boost/python/converter/shared_ptr_deleter.hpp>
#include <boost/version.hpp>

#if BOOST_VERSION >= 103800

namespace boost { namespace detail {

using boost::python::converter::shared_ptr_deleter;

#include <boost/preprocessor/seq/for_each.hpp>
class Artist;
template < typename T > class SceneVariant;
class vec3d;
class QColor;
class Scenario;
class QFont;

// all classes that inherit from boost::python::wrapper need to be listed here (and void)
// all SceneVariants need to be listed here
#define SHOVELART_GIL_PROTECTED_TYPES (void)(Artist)(Scenario)(QFont)(SceneVariant<int>)(SceneVariant<float>)(SceneVariant<vec3d>)(SceneVariant<QColor>)

// BOOST_SP_NOEXCEPT introduced in Boost 1.64, required attribute of dispose()
// since 1.70
#if BOOST_VERSION >= 107000
#define DISPOSE_NOEXCEPT BOOST_SP_NOEXCEPT
#else
#define DISPOSE_NOEXCEPT
#endif

#define SHOVELART_TYPE_DECL_SPECIALIZE( r, _, T ) \
template<> void boost::detail::sp_counted_impl_pd< T*, shared_ptr_deleter>::dispose() DISPOSE_NOEXCEPT; \

BOOST_PP_SEQ_FOR_EACH( SHOVELART_TYPE_DECL_SPECIALIZE, _, SHOVELART_GIL_PROTECTED_TYPES )

#undef SHOVELART_TYPE_DECL_SPECIALIZE

#endif // BOOST_VERSION

}} //namespace boost::detail

#endif // PY_VERSION_HEX >= 0x0207030000

#endif /* I3_SHOVEL_PYCOMPAT_H */
