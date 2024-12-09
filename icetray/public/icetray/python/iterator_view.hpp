// Adapted from boost/python/object/iterator.hpp to return Python3-style views
// into dict keys/values/item.

// Copyright David Abrahams 2002.
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

# pragma once

# include <boost/python/object_core.hpp>
# include <boost/python/object/iterator.hpp>
# include <boost/range/iterator_range.hpp>

namespace icetray { namespace python {

namespace objects {
  namespace bp = boost::python;

  // Instantiations of these are wrapped to produce Python views.
  template <class NextPolicies, class Accessor1, class Accessor2, class Iterator, class Target>
  struct view_range
  {
      view_range(bp::back_reference<Target&> target, Accessor1 get_start, Accessor2 get_finish);

      typedef std::iterator_traits<Iterator> traits_t;

      struct repr
      {
        repr(const char *name) : m_name(name) {}

        bp::str operator()(const bp::object &obj)
        {
          return bp::str("%s(%s)" % bp::make_tuple(m_name, bp::list(obj)));
        }

        const char *m_name;
      };

      typedef repr repr_fn;

      // invoke get_start/get_finish to produce an iterator over the range (whatever it currently is)
      bp::objects::iterator_range<NextPolicies,Iterator> get_iter() const
      {
        bp::objects::detail::demand_iterator_class("iterator", (Iterator*)0, NextPolicies());
        
        return bp::objects::iterator_range<NextPolicies,Iterator>(
            m_target.source()
          , m_get_start(m_target.get())
          , m_get_finish(m_target.get())
        );
      }

      PyObject* get_mapping() const
      {
        return PyDictProxy_New(m_target.source().ptr());
      }

      ssize_t len() const
      {
        return bp::len(m_target.source());
      }

      bool eq(bp::object other) const
      {
        // Unlike CPython's dictview_richcompare, we don't bother checking
        // whether the right-hand side is a set. If it's a sequence of the same
        // length that contains every element we do, then it's equal.
        try {
          if (bp::len(other) != len()) {
            return false;
          }
        } catch (bp::error_already_set&) {
          PyErr_Clear();
          return false;
        }
        for (const auto &item : boost::make_iterator_range(m_get_start(m_target.get()), m_get_finish(m_target.get()))) {
          int ok = PySequence_Contains(other.ptr(), bp::object(item).ptr());
          if (ok < 0) {
            bp::throw_error_already_set();
          } else if (!ok) {
            return false;
          }
        }

        return true;
      }

      bp::back_reference<Target&> m_target; // Keeps the Python object alive, and holds a reference to the C++ object
      Accessor1 m_get_start;
      Accessor2 m_get_finish;
  };

  namespace detail
  {
    // Get a Python class which contains the given iterator accessors and
    // policies, creating it if necessary. Requires: NextPolicies is
    // default-constructible.
    template <class Accessor1, class Accessor2, class Target, class NextPolicies>
    bp::object demand_view_class(
        char const* name
      , Accessor1* = 0
      , Accessor2* = 0
      , Target* = 0
      , NextPolicies const& policies = NextPolicies()
      , bool is_set = true
    )
    {
        typedef typename Accessor1::result_type iterator;
        typedef iterator Iterator; // need to const-qualify this?
        typedef view_range<NextPolicies,Accessor1,Accessor2,Iterator,Target> view_;

        // Check the registry. If one is already registered, return it.
        bp::handle<> class_obj(
            bp::objects::registered_class_object(bp::type_id<view_>()));

        if (class_obj.get() != 0)
            return bp::object(class_obj);

        typedef typename view_::repr_fn repr_fn;

        auto cl = bp::class_<view_>(name, bp::no_init)
            .def("__iter__", &view_::get_iter)
            .def(
                "__repr__"
              , make_function(
                  repr_fn(name)
                , bp::default_call_policies()
                , boost::mpl::vector2<bp::str,bp::object&>()
              ))
            .def(
                "__len__"
              , &view_::len
              )
            .def_readonly(
                "mapping"
              , &view_::get_mapping
              , "map that this view refers to"
              );
        if (is_set) {
          cl.def(
              "__eq__"
            , &view_::eq
          );
        }
        return cl;
    }

  // A function object which builds a view_range.
  template <
      class Target
    , class Iterator
    , class Accessor1
    , class Accessor2
    , class NextPolicies
  >
  struct py_view_
  {
      py_view_(const char *name, Accessor1 const& get_start, Accessor2 const& get_finish, bool is_set)
        : m_name(name)
        , m_get_start(get_start)
        , m_get_finish(get_finish)
        , m_is_set(is_set)
      {}

      // Extract an object x of the Target type from the first Python
      // argument, and invoke get_start(x)/get_finish(x) to produce
      // iterators, which are used to construct a new iterator_range<>
      // object that gets wrapped into a Python iterator.
      view_range<NextPolicies,Accessor1,Accessor2,Iterator,Target>
      operator()(bp::back_reference<Target&> x) const
      {
          // Make sure the Python class is instantiated.
          detail::demand_view_class(
              m_name
            , (Accessor1*)0
            , (Accessor2*)0
            , (Target*)0
            , NextPolicies()
            , m_is_set
          );

          return view_range<
              NextPolicies
            , Accessor1
            , Accessor2
            , Iterator
            , Target
          >(
              x
            , m_get_start
            , m_get_finish
          );
      }
   private:
      const char *m_name;
      Accessor1 m_get_start;
      Accessor2 m_get_finish;
      bool m_is_set;
  };

  template <class Target, class Iterator, class NextPolicies, class Accessor1, class Accessor2>
  inline bp::object make_view_function(
    const char *name
    , Accessor1 const& get_start
    , Accessor2 const& get_finish
    , bool is_set
    , NextPolicies const& /*next_policies*/
    , Iterator const& (*)()
    , boost::type<Target>*
    , int
  )
  {
      return make_function(
          py_view_<Target,Iterator,Accessor1,Accessor2,NextPolicies>(name, get_start, get_finish, is_set)
        , bp::default_call_policies()
        , boost::mpl::vector2<view_range<NextPolicies,Accessor1,Accessor2,Iterator,Target>, bp::back_reference<Target&> >()
      );
  }

  template <class Target, class Iterator, class NextPolicies, class Accessor1, class Accessor2>
  inline bp::object make_view_function(
    const char *name
    , Accessor1 const& get_start
    , Accessor2 const& get_finish
    , bool is_set
    , NextPolicies const& next_policies
    , Iterator& (*)()
    , boost::type<Target>*
    , ...)
  {
      return make_view_function(
        name
        , get_start
        , get_finish
        , is_set
        , next_policies
        , (Iterator const&(*)())0
        , (boost::type<Target>*)0
        , 0
      );
  }
}

  // Create a Python callable object which accepts a single argument
  // convertible to the C++ Target type and returns a Python
  // view. The Python view uses get_start(x) and get_finish(x)
  // (where x is an instance of Target) to produce begin and end
  // iterators for the range, and an instance of NextPolicies is used as
  // CallPolicies for the Python iterator's next() function.
  template <class Target, class NextPolicies, class Accessor1, class Accessor2>
  inline bp::object make_view_function(
    const char *name
    , Accessor1 const& get_start
    , Accessor2 const& get_finish
    , bool is_set
    , NextPolicies const& next_policies
    , boost::type<Target>* = 0
  )
  {
      typedef typename Accessor1::result_type iterator;
      typedef typename boost::python::detail::add_const<iterator>::type iterator_const;
      typedef typename boost::python::detail::add_lvalue_reference<iterator_const>::type iterator_cref;

      return detail::make_view_function(
        name
        , get_start
        , get_finish
        , is_set
        , next_policies
        , (iterator_cref(*)())0
        , (boost::type<Target>*)0
        , 0
      );
  }

  //
  // implementation
  //
  template <class NextPolicies, class Accessor1, class Accessor2, class Iterator, class Target>
  inline view_range<NextPolicies,Accessor1,Accessor2,Iterator,Target>::view_range(
    bp::back_reference<Target&> target, Accessor1 get_start, Accessor2 get_finish
  )
      : m_target(target), m_get_start(get_start), m_get_finish(get_finish)
  {
  }

}

namespace detail {
  // Adds an additional layer of binding to
  // objects::make_iterator(...), which allows us to pass member
  // function and member data pointers.
  template <class Target, class Accessor1, class Accessor2, class NextPolicies>
  inline boost::python::object make_view(
    const char *name
    , Accessor1 get_start
    , Accessor2 get_finish
    , bool is_set
    , NextPolicies next_policies
    , Target&(*)()
  )
  {
      using namespace boost::placeholders;
      return objects::make_view_function<Target>(
        name
        , boost::protect(boost::bind(get_start, _1))
        , boost::protect(boost::bind(get_finish, _1))
        , is_set
        , next_policies
      );
  }
}

// Create an iterator-building function which uses the given
// accessors. Deduce the Target type from the accessors. The iterator
// returns copies of the inderlying elements.
template <class Accessor1, class Accessor2>
boost::python::object range_view(const char *name, Accessor1 start, Accessor2 finish)
{
    return detail::make_view(
        name, start, finish, false
      , boost::python::objects::default_iterator_call_policies()
      , boost::python::detail::target(start)
    );
}

}}
