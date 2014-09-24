
// This visitor class exposes operators to Python 
// Usage is similar to the indexing suites:
//
// class_<Foo>("Foo")
//   .def(operator_suite<Foo>())
// ;

#ifndef ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED
#define ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED

#include <boost/preprocessor/cat.hpp>
#include <boost/python/def_visitor.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#if BOOST_VERSION>104800
#include <boost/type_traits/has_operator.hpp>
#endif

namespace bp = boost::python;

namespace boost { namespace python {

namespace operator_detail {

#if BOOST_VERSION>104800
// boost type_traits operator detection sometimes fails on stl types
// because the operators do not implement SFINAE, so check for that

#define HAS_OPERATOR(func, name)                                        \
  template<typename T>                                                  \
  struct name {                                                         \
    template<typename TT,typename Dummy=void>                           \
    struct chkAlloc : public boost::true_type{};                        \
    template<typename TT>                                               \
    struct chkAlloc<TT,typename boost::mpl::if_c<false,typename TT::allocator_type,void>::type> \
    {                                                                   \
      static bool const value = (name<typename TT::allocator_type>::value); \
    };                                                                  \
    template<typename TT,typename Dummy=void>                           \
    struct chkMap : public boost::true_type{};                          \
    template<typename TT>                                               \
    struct chkMap<TT,typename boost::mpl::if_c<false,typename TT::key_type,void>::type> \
    {                                                                   \
      static bool const value = (name<typename TT::key_type>::value &&  \
                                 name<typename TT::mapped_type>::value); \
    };                                                                  \
    template<typename TT,typename Dummy=void>                           \
    struct chk : public boost::true_type{};                             \
    template<typename TT>                                               \
    struct chk<TT,typename boost::mpl::if_c<false,typename TT::value_type,void>::type> \
    {                                                                   \
      static bool const value = (chkAlloc<TT>::value && chkMap<TT>::value \
                                 && name<typename TT::value_type>::value); \
    };                                                                  \
    static bool const value = (chk<T>::value && func<T>::value);        \
  }

HAS_OPERATOR(boost::has_equal_to, has_eq);
HAS_OPERATOR(boost::has_not_equal_to, has_ne);
HAS_OPERATOR(boost::has_less, has_lt);
HAS_OPERATOR(boost::has_less_equal, has_lt_eq);
HAS_OPERATOR(boost::has_greater, has_gt);
HAS_OPERATOR(boost::has_greater_equal, has_gt_eq);

#undef HAS_OPERATOR

#else
// older boost, so do operator detection manually
// note that this only works for operators that are member functions

#define HAS_OPERATOR(func, ret, name)                                   \
    template<typename T>                                                \
    struct name {                                                       \
        typedef char yes[1];                                            \
        typedef char no [2];                                            \
        template <typename U, U> struct type_check;                     \
        template <typename _1> static yes &chk(type_check<ret(T::*)(const T&) const, &_1::func> *); \
        template <typename   > static no  &chk(...);                    \
        template <typename _1> static yes &chk2(type_check<ret(T::*)(const T&), &_1::func> *); \
        template <typename   > static no  &chk2(...);                   \
        static bool const value = (sizeof(chk<T>(0)) == sizeof(yes)     \
                                || sizeof(chk2<T>(0)) == sizeof(yes));  \
    }

HAS_OPERATOR(operator==, bool, has_eq);
HAS_OPERATOR(operator!=, bool, has_ne);
HAS_OPERATOR(operator<, bool, has_lt);
HAS_OPERATOR(operator<=, bool, has_lt_eq);
HAS_OPERATOR(operator>, bool, has_gt);
HAS_OPERATOR(operator>=, bool, has_gt_eq);

#undef HAS_OPERATOR

#endif

}

#define LIST_OF_OPERATORS (eq)(ne)(lt)(lt_eq)(gt)(gt_eq)
#define MAP_OF_OPERATORS(T)                                             \
( (eq)(==)(T) )( (ne)(!=)(T) )( (lt)(<)(T) )( (lt_eq)(<=)(T) )( (gt)(>)(T) )( (gt_eq)(>=)(T) )

#define OPERATOR_CALL(r, data, OP) BOOST_PP_CAT(add_, OP) <Class, T>(cl);

#define OPERATOR_DEF(r, data, OP)                                       \
template <class Class, typename U>                                      \
static                                                                  \
typename boost::enable_if_c<operator_detail:: BOOST_PP_CAT(has_,BOOST_PP_SEQ_ELEM(0,OP)) <U>::value>::type \
BOOST_PP_CAT(add_, BOOST_PP_SEQ_ELEM(0, OP) ) (Class &cl) {             \
  cl.def(self BOOST_PP_SEQ_ELEM(1, OP) BOOST_PP_SEQ_ELEM(2, OP) );      \
}                                                                       \
template <class Class, typename U>                                      \
static                                                                  \
typename boost::disable_if_c<operator_detail:: BOOST_PP_CAT(has_,BOOST_PP_SEQ_ELEM(0,OP)) <U>::value>::type \
BOOST_PP_CAT(add_, BOOST_PP_SEQ_ELEM(0, OP) ) (Class &cl) { }

#define OPERATOR_TYPE_SUITE(NAME, TYPE)                                 \
template <class T>                                                      \
class NAME : public bp::def_visitor< NAME <T > > {                      \
  public:                                                               \
    template <class Class>                                              \
    static void                                                         \
    visit(Class& cl)                                                    \
    {                                                                   \
      BOOST_PP_SEQ_FOR_EACH(OPERATOR_CALL, _, LIST_OF_OPERATORS )       \
    }                                                                   \
  private:                                                              \
    BOOST_PP_SEQ_FOR_EACH(OPERATOR_DEF, _, MAP_OF_OPERATORS( TYPE ) ) \
};

OPERATOR_TYPE_SUITE(operator_suite, self)
OPERATOR_TYPE_SUITE(operator_bool_suite, bool() )
OPERATOR_TYPE_SUITE(operator_int_suite, int() )
OPERATOR_TYPE_SUITE(operator_float_suite, float() )

#undef LIST_OF_OPERATORS
#undef MAP_OF_OPERATORS
#undef OPERATOR_CALL
#undef OPERATOR_DEF
#undef OPERATOR_TYPE_SUITE

}} // namespace boost::python

#endif // ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED

