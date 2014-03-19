
// This visitor class exposes operators to Python 
// Usage is similar to the indexing suites:
//
// class_<Foo>("Foo")
//   .def(operator_suite<Foo>())
// ;

#ifndef ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED
#define ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED

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

HAS_OPERATOR(boost::has_equal_to, has_equal_to);
HAS_OPERATOR(boost::has_not_equal_to, has_not_equal_to);
HAS_OPERATOR(boost::has_less, has_less);
HAS_OPERATOR(boost::has_less_equal, has_less_equal);
HAS_OPERATOR(boost::has_greater, has_greater);
HAS_OPERATOR(boost::has_greater_equal, has_greater_equal);

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

HAS_OPERATOR(operator==, bool, has_equal_to);
HAS_OPERATOR(operator!=, bool, has_not_equal_to);
HAS_OPERATOR(operator<, bool, has_less);
HAS_OPERATOR(operator<=, bool, has_less_equal);
HAS_OPERATOR(operator>, bool, has_greater);
HAS_OPERATOR(operator>=, bool, has_greater_equal);

#undef HAS_OPERATOR

#endif

}

template <class T>
class operator_suite : public bp::def_visitor<operator_suite<T > > {
  public:
    template <class Class>
    static void
    visit(Class& cl)
    {
      add_eq<Class, T>(cl);
      add_ne<Class, T>(cl);
      add_lt<Class, T>(cl);
      add_lt_eq<Class, T>(cl);
      add_gt<Class, T>(cl);
      add_gt_eq<Class, T>(cl);
    }
  private:
    // operator==
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_equal_to<U>::value>::type
    add_eq(Class &cl)
    {
      cl.def(self == self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_equal_to<U>::value>::type
    add_eq(Class &cl) {}
    
    // operator!=
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_not_equal_to<U>::value>::type
    add_ne(Class &cl)
    {
      cl.def(self == self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_not_equal_to<U>::value>::type
    add_ne(Class &cl) {}
    
    // operator <
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_less<U>::value>::type
    add_lt(Class &cl)
    {
      cl.def(self < self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_less<U>::value>::type
    add_lt(Class &cl) {}
    
    // operator <=
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_less_equal<U>::value>::type
    add_lt_eq(Class &cl)
    {
      cl.def(self <= self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_less_equal<U>::value>::type
    add_lt_eq(Class &cl) {}
    
    // operator >
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_greater<U>::value>::type
    add_gt(Class &cl)
    {
      cl.def(self > self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_greater<U>::value>::type
    add_gt(Class &cl) {}
    
    // operator >=
    template <class Class, typename U>
    static
    typename boost::enable_if_c<operator_detail::has_greater_equal<U>::value>::type
    add_gt_eq(Class &cl)
    {
      cl.def(self >= self);
    }
    template <class Class, typename U>
    static
    typename boost::disable_if_c<operator_detail::has_greater_equal<U>::value>::type
    add_gt_eq(Class &cl) {}
};

}} // namespace boost::python

#endif // ICETRAY_PYTHON_OPERATOR_SUITE_HPP_INCLUDED

