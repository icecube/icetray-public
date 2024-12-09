//  (C) Copyright Joel de Guzman 2003.
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//  Modified by Troy D. Straszheim and Jakob van Santen, 2009-03-26

#ifndef ICETRAY_PYTHON_STD_MAP_INDEXING_SUITE_HPP_INCLUDED
#define ICETRAY_PYTHON_STD_MAP_INDEXING_SUITE_HPP_INCLUDED

# include <boost/python/suite/indexing/indexing_suite.hpp>
# include <boost/python/iterator.hpp>
# include <boost/python/call_method.hpp>
# include <boost/python/tuple.hpp>
# include <boost/python/overloads.hpp>
# include <boost/iterator/transform_iterator.hpp>
# include <icetray/python/get_class.hpp>
# include <icetray/python/iterator_view.hpp>

namespace bp = boost::python;

namespace boost { namespace python {

    // Forward declaration
    template <class Container, bool NoProxy, class DerivedPolicies>
    class std_map_indexing_suite;

    namespace detail
    {
        template <class Container, bool NoProxy>
        class final_std_map_derived_policies
            : public std_map_indexing_suite<Container,
                NoProxy, final_std_map_derived_policies<Container, NoProxy> > {};

#define HAS_TYPEDEF(def, name)                                                 \
    template<typename T, typename Dummy=void>                                  \
    struct name : public boost::false_type{};                                  \
    template<typename T>                                                       \
    struct name<T,typename boost::mpl::if_c<false,typename T::def,void>::type> \
    : public boost::true_type{};

        HAS_TYPEDEF(hasher, is_hash_map);

#undef HAS_TYPEDEF

#define HAS_FUNCTION(func,result_type,name)                         \
    namespace func ## test {                                        \
        template <typename U, U> class check { };                   \
        template <typename C>                                       \
        static char f(check<typename C::result_type (C::*)() const, \
                      &C::func>*);                                  \
        template <typename C> static long f(...);                   \
    }                                                               \
    template<typename T, typename Dummy=void>                       \
    struct name : public boost::false_type{};                       \
    template <typename T>                                           \
    struct name<T,typename boost::mpl::if_c<                        \
        sizeof(func ## test :: f<T>(0)) != sizeof(char),            \
        int,void>::type>                                            \
    : public boost::true_type{};

        HAS_FUNCTION(hash_funct,hasher,is_ext_hash_map);
        HAS_FUNCTION(hash_function,hasher,is_std_unordered_map);

#undef HAS_FUNCTION

        template<typename HashMap>
        static
        typename boost::enable_if<
            is_ext_hash_map<HashMap>,
            typename HashMap::hasher
        >::type
        get_hasher(const HashMap& m){
            return m.hash_funct();
        }

        template<typename HashMap>
        static
        typename boost::enable_if<
            is_std_unordered_map<HashMap>,
            typename HashMap::hasher
        >::type
        get_hasher(const HashMap& m){
            return m.hash_function();
        }

        template<typename Map>
        static
        typename boost::disable_if<
            is_hash_map<Map>,
            bool
        >::type
        compare_keys(Map& m, typename Map::key_type a, typename Map::key_type b){
            return m.key_comp()(a, b);
        }

        template<typename Map>
        static
        typename boost::enable_if<
            is_hash_map<Map>,
            bool
        >::type
        compare_keys(Map& m, typename Map::key_type a, typename Map::key_type b){
        typename Map::hasher hash = get_hasher(m);
            if(hash(a)!=hash(b))
                return false;
            return m.key_eq()(a, b);
        }
    }

    // The map_indexing_suite class is a predefined indexing_suite derived
    // class for wrapping std::vector (and std::vector like) classes. It provides
    // all the policies required by the indexing_suite (see indexing_suite).
    // Example usage:
    //
    //  class X {...};
    //
    //  ...
    //
    //      class_<std::map<std::string, X> >("XMap")
    //          .def(map_indexing_suite<std::map<std::string, X> >())
    //      ;
    //
    // By default indexed elements are returned by proxy. This can be
    // disabled by supplying *true* in the NoProxy template parameter.
    //
    template <
        class Container,
        bool NoProxy = false,
        class DerivedPolicies
            = detail::final_std_map_derived_policies<Container, NoProxy> >
    class std_map_indexing_suite
        : public indexing_suite<
            Container
          , DerivedPolicies
          , NoProxy
          , true
          , typename Container::value_type::second_type
          , typename Container::key_type
          , typename Container::key_type
        >
    {
    public:

        typedef typename Container::value_type value_type;
        typedef typename Container::value_type::second_type data_type;
        typedef typename Container::key_type key_type;
        typedef typename Container::key_type index_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::difference_type difference_type;
        typedef typename Container::const_iterator const_iterator;

        // return references to values if they are classes (but not shared_ptr) unless NoProxy was set
        typedef typename mpl::and_<
            is_class<data_type>
            , mpl::not_<
                mpl::or_<
                    mpl::bool_<NoProxy>
                  , detail::is_shared_ptr<data_type>
                >
            >
        >::type return_value_by_proxy;

        static bp::object get_key_type()
        {
          return get_class<key_type>();
        }
        static bp::object get_value_type()
        {
          return get_class<data_type>();
        }

        // get with default value
        static object dict_get(Container const& x, index_type const& k, object const& default_val = object())
        {
            const_iterator it = x.find(k);
            if (it != x.end()) return object(it->second);
            else return default_val;
        }
        // preserve default value info
        BOOST_PYTHON_FUNCTION_OVERLOADS(dict_get_overloads, dict_get, 2, 3);

        // pop map[key], or throw an error if it doesn't exist
        static object dict_pop(Container & x, index_type const& k)
        {
            const_iterator it = x.find(k);
            object result;
            if (it != x.end()) {
                result = object(it->second);
                x.erase(it->first);
                return result;
            }
            else {
                PyErr_SetString(PyExc_KeyError,"Key not found.");
                throw_error_already_set();
                return object(); // None
            };
        }

        // pop map[key], or return default_val if it doesn't exist
        static object dict_pop_default(Container & x, index_type const& k, object const& default_val)
        {
            const_iterator it = x.find(k);
            object result;
            if (it != x.end()) {
                result = object(it->second);
                x.erase(it->first);
                return result;
            }
            else return default_val;
        }

        // pop a tuple, or throw an error if empty
        static object dict_pop_item(Container & x)
        {
            const_iterator it = x.begin();
            object result;
            if (it != x.end()) {
                result = bp::make_tuple(it->first,it->second);
                x.erase(it->first);
                return result;
            }
            else {
                PyErr_SetString(PyExc_KeyError,"No more items to pop");
                throw_error_already_set();
                return object(); // None
            };
        }

        // create a new map with given keys, initialized to value
        static object dict_fromkeys(object const& keys, object const& value)
        {
            object newmap = object(Container());
            object keys_iter = keys.attr("__iter__")();
            while (true) { // 'cuz python is more fun in C++...
                try {
                    object key = keys_iter.attr("__next__")();
                    newmap.attr("__setitem__")(key,value);
                } catch (error_already_set &err) {
                    if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                        PyErr_Clear();
                        break;
                    }
                    throw;
                }
            }
            return newmap;
        }

		// spice up the constructors a bit
        template <typename PyClassT>
        struct init_factory {
            typedef typename PyClassT::metadata::holder Holder;
            typedef bp::objects::instance<Holder> instance_t;

            // connect the PyObject to a wrapped C++ instance
            // borrowed from boost/python/object/make_holder.hpp
            static void make_holder(PyObject *p)
            {
                void* memory = Holder::allocate(p, offsetof(instance_t, storage), sizeof(Holder));

                try {
                    // this only works for blank () constructors
                    (new (memory) Holder(p))->install(p);
                }
                catch(...) {
                    Holder::deallocate(p, memory);
                    throw;
                }
            }
            static void from_dict(PyObject *p, bp::dict const& dict)
            {
                make_holder(p);
                object newmap = object(bp::handle<>(borrowed(p)));
                newmap.attr("update")(dict);
            }
            static void from_list(PyObject *p, bp::list const& list)
            {
                make_holder(p);
                object newmap = object(bp::handle<>(borrowed(p)));
                newmap.attr("update")(bp::dict(list));
            }
        };

        // copy keys and values from dictlike object (anything with keys())
        static void dict_update(object & x, object const& dictlike)
        {
            object key;
            object keys = dictlike.attr("keys")();
            int numkeys = extract<int>(keys.attr("__len__")());
            object keys_iter = keys.attr("__iter__")();
            for(int i=0;i<numkeys;i++) {
                key = keys_iter.attr("__next__")();
                x.attr("__setitem__")(key,dictlike.attr("__getitem__")(key));
            }

        }

        // set up operators to extract the key, value, or a tuple from a std::pair
        struct iterkeys
        {
          typedef key_type result_type;

          result_type operator()(value_type const& x) const
          {
            return x.first;
          }
        };

        struct itervalues
        {
          typedef typename mpl::if_<
              return_value_by_proxy
            , data_type&
            , data_type
          >::type result_type;

          result_type operator()(value_type& x) const
          {
            return x.second;
          }
        };

        struct iteritems {
          typedef tuple result_type;

          // pass non-POD values back to python as references
          typedef typename mpl::if_<
              return_value_by_proxy
            , boost::reference_wrapper<data_type>
            , data_type
          >::type maybe_ref;

          result_type operator()(value_type & x) const
          {
            return bp::make_tuple(x.first, maybe_ref(x.second));
          }
        };

        template <typename Transform>
        struct make_transform_impl
        {
          typedef boost::transform_iterator<Transform, typename Container::iterator> iterator;
          // return references as internal references, values as copies
          typedef typename mpl::if_c<
            ::boost::is_reference<typename Transform::result_type>::value
            , bp::return_internal_reference<>
            , bp::objects::default_iterator_call_policies
            >::type call_policies;

          static iterator begin(Container& m)
          {
            return boost::make_transform_iterator(m.begin(), Transform());
          }
          static iterator end(Container& m)
          {
            return boost::make_transform_iterator(m.end(), Transform());
          }

          static bp::object view(const char *name, bool is_set)
          {
            // NB: use detail form to set call policy explicitly
            return icetray::python::detail::make_view(
                  name
                , &begin
                , &end
                , is_set
                , call_policies()
                , bp::detail::target(&begin)
            );
          }

          static bp::object range()
          {
            return bp::range(&begin, &end);
          }
        };

        template <typename Transform>
        static bp::object
        make_view(const char *name, bool is_set=false)
        {
          return make_transform_impl<Transform>::view(name, is_set);
        }

        template <typename Transform>
        static bp::object
        make_range()
        {
          return make_transform_impl<Transform>::range();
        }

        static data_type&
        get_item(Container& container, index_type i_)
        {
            typename Container::iterator i = container.find(i_);
            if (i == container.end())
            {
                PyErr_SetString(PyExc_KeyError, "Invalid key");
                throw_error_already_set();
            }
            return i->second;
        }

        static void
        set_item(Container& container, index_type i, data_type const& v)
        {
            container[i] = v;
        }

        static void
        delete_item(Container& container, index_type i)
        {
            container.erase(i);
        }

        static size_t
        size(Container& container)
        {
            return container.size();
        }

        static bool
        contains(Container& container, key_type const& key)
        {
            return container.find(key) != container.end();
        }

        static bool
        compare_index(Container& container, index_type a, index_type b)
        {
            return detail::compare_keys(container, a, b);
        }

        static index_type
        convert_index(Container& container, PyObject* i_)
        {
            extract<key_type const&> i(i_);
            if (i.check())
            {
                return i();
            }
            else
            {
                extract<key_type> i(i_);
                if (i.check())
                    return i();
            }

            PyErr_SetString(PyExc_TypeError, "Invalid index type");
            throw_error_already_set();
            return index_type();
        }

        template <class Class>
        static void
        extension_def(Class& cl)
        {
            std::string cl_name = extract<std::string>(cl.attr("__name__"));

            // add convenience methods to the map
            cl
                // declare constructors in descending order of arity
                .def("__init__", init_factory<Class>::from_list,
                 "Initialize with keys and values from a Python dictionary: {'key':'value'}\n")
                .def("__init__", init_factory<Class>::from_dict,
                 "Initialize with keys and values as tuples in a Python list: [('key','value')]\n")
                .def(init<>()) // restore default constructor

                .def("clear", &Container::clear, "D.clear() -> None.  Remove all items from D.\n")
                .def("get", dict_get, dict_get_overloads(args("default_val"),
                 "D.get(k[,d]) -> D[k] if k in D, else d.  d defaults to None.\n"))
                .def("pop", &dict_pop )
                .def("pop", &dict_pop_default,
                 "D.pop(k[,d]) -> v, remove specified key and return the corresponding value\nIf key is not found, d is returned if given, otherwise KeyError is raised\n")
                .def("popitem", &dict_pop_item,
                 "D.popitem() -> (k, v), remove and return some (key, value) pair as a\n2-tuple; but raise KeyError if D is empty\n")
                .def("update", &dict_update,
                 "D.update(E) -> None.  Update D from E: for k in E: D[k] = E[k]\n")
                .def("__iter__",
                 make_range<iterkeys>())
                .def("items",
                 make_view<iteritems>("map_items", true),
                 "an iterator over the (key, value) items of D\n")
                .def("keys",
                 make_view<iterkeys>("map_keys", true),
                 "an iterator over the keys of D\n")
                .def("values",
                 make_view<itervalues>("map_values", false), // values are not a set
                 "an iterator over the values of D\n")
                .def("__key_type__", &get_key_type)
                .staticmethod("__key_type__")
                .def("__value_type__", &get_value_type)
                .staticmethod("__value_type__")
              ;
        }

    };

}} // namespace boost::python

#endif // ICETRAY_PYTHON_STD_MAP_INDEXING_SUITE_HPP_INCLUDED
