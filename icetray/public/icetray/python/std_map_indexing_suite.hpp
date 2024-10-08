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


        // __getitem__ for std::pair
// FIXME: horrible (20x) performance regression vs. (pair.key(),pair.data())
        static object pair_getitem(value_type const& x, int i) {
            if (i==0 || i==-2) return object(x.first);
            else if (i==1 || i==-1) return object(x.second);
            else {
                PyErr_SetString(PyExc_IndexError,"Index out of range.");
                throw_error_already_set();
                return object(); // None
            }
        }

// __iter__ for std::pair
// here we cheat by making a tuple and returning its iterator
// FIXME: replace this with a pure C++ iterator
// how to handle the different return types of first and second?
static PyObject* pair_iter(value_type const& x) {
object tuple = bp::make_tuple(x.first,x.second);
return incref(tuple.attr("__iter__")().ptr());
}

        // __len__ std::pair = 2
        static int pair_len(value_type const& x) { return 2; }

        // return a list of keys
        static bp::list keys(Container const&  x)
        {
          bp::list t;
          for(typename Container::const_iterator it = x.begin(); it != x.end(); it++)
            t.append(it->first);
          return t;
        }
        // return a list of values
        static bp::list values(Container const&  x)
        {
          bp::list t;
          for(typename Container::const_iterator it = x.begin(); it != x.end(); it++)
            t.append(it->second);
          return t;
        }
        // return a list of (key,value) tuples
        static bp::list items(Container const&  x)
        {
          bp::list t;
          for(typename Container::const_iterator it = x.begin(); it != x.end(); it++)
            t.append(bp::make_tuple(it->first, it->second));
          return t;
        }
        static bp::object get_key_type()
        {
          return get_class<key_type>();
        }
        static bp::object get_value_type()
        {
          return get_class<data_type>();
        }
        static bp::object get_item_type()
        {
          return get_class<value_type>();
        }

        // return a shallow copy of the map
        // FIXME: is this actually a shallow copy, or did i duplicate the pairs?
        static Container copy(Container const& x)
        {
            Container newmap = Container();
            for(const_iterator it = x.begin();it != x.end();it++) newmap.insert(*it);
            return newmap;
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
            int numkeys = extract<int>(keys.attr("__len__")());
            object keys_iter = keys.attr("__iter__")();
            for(int i=0;i<numkeys;i++) { // 'cuz python is more fun in C++...
                object key = keys_iter.attr("__next__")();
                newmap.attr("__setitem__")(key,value);
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
          typedef data_type result_type;

          result_type operator()(value_type const& x) const
          {
            return x.second;
          }
        };

        struct iteritems {
          typedef tuple result_type;

          result_type operator()(value_type const& x) const
          {
            return bp::make_tuple(x.first,x.second);
          }
        };

        template <typename Transform>
        struct make_transform_impl
        {
          typedef boost::transform_iterator<Transform, const_iterator> iterator;

          static iterator begin(const Container& m)
          {
            return boost::make_transform_iterator(m.begin(), Transform());
          }
          static iterator end(const Container& m)
          {
            return boost::make_transform_iterator(m.end(), Transform());
          }

          static bp::object range()
          {
            return bp::range(&begin, &end);
          }
        };

        template <typename Transform>
        static bp::object
        make_transform()
        {
          return make_transform_impl<Transform>::range();
        }

        static python::str
        print_elem(typename Container::value_type const& e)
        {
            return python::str("(%s, %s)" % python::make_tuple(e.first, e.second));
        }

        static
        typename mpl::if_<
            is_class<data_type>
          , data_type&
          , data_type
        >::type
        get_data(typename Container::value_type& e)
        {
            return e.second;
        }

        static typename Container::key_type
        get_key(typename Container::value_type& e)
        {
            return e.first;
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
            //  Wrap the map's element (value_type)
            std::string elem_name = "std_map_indexing_suite_";
            std::string cl_name;
            object class_name(cl.attr("__name__"));
            extract<std::string> class_name_extractor(class_name);
            cl_name = class_name_extractor();
            elem_name += cl_name;
            elem_name += "_entry";

            typedef typename mpl::if_<
                is_class<data_type>
              , return_internal_reference<>
              , default_call_policies
            >::type get_data_return_policy;

            if (!get_class<value_type>()) {
                class_<value_type>(elem_name.c_str())
                    .def("__repr__", &DerivedPolicies::print_elem)
                    .def("data", &DerivedPolicies::get_data, get_data_return_policy(),
                    "K.data() -> the value associated with this pair.\n")
                    .def("key", &DerivedPolicies::get_key,
                    "K.key() -> the key associated with this pair.\n")
                    .def("__getitem__",&pair_getitem)
                    .def("__iter__",&pair_iter)
                    .def("__len__",&pair_len)
                    .def("first",&DerivedPolicies::get_key,
                    "K.first() -> the first item in this pair.\n")
                    .def("second",&DerivedPolicies::get_data, get_data_return_policy(),
                    "K.second() -> the second item in this pair.\n")
                ;
            }
            // add convenience methods to the map

            cl
                // declare constructors in descending order of arity
                .def("__init__", init_factory<Class>::from_list,
                 "Initialize with keys and values from a Python dictionary: {'key':'value'}\n")
                .def("__init__", init_factory<Class>::from_dict,
                 "Initialize with keys and values as tuples in a Python list: [('key','value')]\n")
                .def(init<>()) // restore default constructor

                .def("keys", &keys, "list of D's keys\n")
                .def("has_key", &contains, "D.has_key(k) -> True if D has a key k, else False\n") // don't re-invent the wheel
                .def("values", &values, "D.values() -> list of D's values\n")
                .def("items", &items, "D.items() -> list of D's (key, value) pairs, as 2-tuples\n")
                .def("clear", &Container::clear, "D.clear() -> None.  Remove all items from D.\n")
                .def("copy", &copy, "a shallow copy of D\n")
                .def("get", dict_get, dict_get_overloads(args("default_val"),
                 "D.get(k[,d]) -> D[k] if k in D, else d.  d defaults to None.\n"))
                .def("pop", &dict_pop )
                .def("pop", &dict_pop_default,
                 "D.pop(k[,d]) -> v, remove specified key and return the corresponding value\nIf key is not found, d is returned if given, otherwise KeyError is raised\n")
                .def("popitem", &dict_pop_item,
                 "D.popitem() -> (k, v), remove and return some (key, value) pair as a\n2-tuple; but raise KeyError if D is empty\n")
                .def("fromkeys", &dict_fromkeys,
                 (cl_name+".fromkeys(S,v) -> New "+cl_name+" with keys from S and values equal to v.\n").c_str())
                .staticmethod("fromkeys")
                .def("update", &dict_update,
                 "D.update(E) -> None.  Update D from E: for k in E: D[k] = E[k]\n")
                .def("iteritems",
                 make_transform<iteritems>(),
                 "an iterator over the (key, value) items of D\n")
                .def("iterkeys",
                 make_transform<iterkeys>(),
                 "an iterator over the keys of D\n")
                .def("itervalues",
                 make_transform<itervalues>(),
                 "an iterator over the values of D\n")
                .def("__key_type__", &get_key_type)
                .staticmethod("__key_type__")
                .def("__value_type__", &get_value_type)
                .staticmethod("__value_type__")
                .def("__item_type__", &get_item_type)
                .staticmethod("__item_type__")
              ;
        }

    };

}} // namespace boost::python

#endif // ICETRAY_PYTHON_STD_MAP_INDEXING_SUITE_HPP_INCLUDED
