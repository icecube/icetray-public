//  dataclass_suite: provide all the goodies dataclasses should have in
//                   a single visitor, including:
//  - indexing (for maps, vectors, and trees)
//  - copy constructors
//  - __str__ representations
//  - boost::serialization-backed pickling
//  - disabling __setattr__ to avoid API confusion
//  
//  (C) 2012 Jakob van Santen <vansanten@wisc.edu>
//           and the IceCube Collaboration



#include <boost/python/def_visitor.hpp>

#include <icetray/python/copy_suite.hpp>
#include <icetray/python/stream_to_string.hpp>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/std_map_indexing_suite.hpp>
#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/tree_indexing_suite.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>

#include <boost/static_assert.hpp>
#include <boost/typeof/typeof.hpp>

namespace boost { namespace python {

namespace detail {

#define HAS_TYPEDEF(def, name)                                                 \
	template<typename T, typename Dummy=void>                                  \
	struct name : public boost::false_type{};                                  \
	template<typename T>                                                       \
	struct name<T,typename boost::mpl::if_c<false,typename T::def,void>::type> \
	: public boost::true_type{};

HAS_TYPEDEF(iterator, has_iterator);
HAS_TYPEDEF(fixed_depth_iterator, is_tree);
HAS_TYPEDEF(key_type, is_map);

#undef HAS_TYPEDEF

// Catch-all for things that have an iterator typedef,
// but are neither trees nor maps
template <typename T>
struct is_iterable {
	static const bool value = has_iterator<T>::value && !(is_map<T>::value || is_tree<T>::value);
};

namespace has_operator {
    typedef char yes;
    typedef struct { char array[2]; } no;
    
    struct anyx { template <class T> anyx(const T &); };
    no operator << (const anyx &, const anyx &);
    
    template <class T> yes check(T const&);
    no check(no);
    
    template <typename StreamType, typename T>
    struct insertion {
        static StreamType & stream;
        static T & x;
        static const bool value = sizeof(check(stream << x)) == sizeof(yes);
    };
}

}
	
template <class T>
class dataclass_suite : public bp::def_visitor<dataclass_suite<T > > {
private:
	
	static
	bp::str
	list_repr(T &seq)
	{
		bp::list l(seq);
		return bp::extract<bp::str>(l.attr("__repr__")());
	}
	
	template <class Class, typename U>
	static
	typename boost::enable_if_c<detail::is_map<U>::value>::type
	add_indexing(Class &cl)
	{
		cl.def(std_map_indexing_suite<U>());
	}
	
	template <class Class, typename U>
	static
	typename boost::enable_if_c<detail::is_iterable<U>::value>::type
	add_indexing(Class &cl)
	{
		cl.def(list_indexing_suite<U>());
		cl.def("__repr__", &list_repr);
		using namespace scitbx::boost_python::container_conversions;
		from_python_sequence<U, variable_capacity_policy>();
	}
	
	template <class Class, typename U>
	static
	typename boost::enable_if_c<detail::is_tree<U>::value>::type
	add_indexing(Class &cl)
	{
		cl.def(tree_indexing_suite<U>());
	}
	
	template <class Class, typename U>
	static
	typename boost::disable_if<boost::mpl::or_<
		boost::mpl::bool_<detail::is_map<U>::value>,
		boost::mpl::bool_<detail::is_iterable<U>::value>,
		boost::mpl::bool_<detail::is_tree<U>::value> > >::type
	add_indexing(Class &cl) {}
	
	template <class Class, typename U>
	static
	typename boost::enable_if_c<detail::has_operator::insertion<std::ostream, U>::value>::type
	add_string_to_stream(Class &cl)
	{
		cl.def("__str__", &stream_to_string<U>);
	}

	template <class Class, typename U>
	static
	typename boost::disable_if_c<detail::has_operator::insertion<std::ostream, U>::value>::type
	add_string_to_stream(Class &cl) {}
	
public:
	template <class Class>
	static void
	visit(Class& cl)
	{
		// Add any must-have visitors here. Any that rely on non-universal
		// class attributes (like the indexing suites) should be
		// conditionalized to fail gracefully, following the SFINAE
		// pattern above.
		cl.def(copy_suite<T>());
		cl.def_pickle(boost_serializable_pickle_suite<T>());
		add_indexing<Class, T>(cl);
		add_string_to_stream<Class, T>(cl);
		cl.def(freeze());
	}
	
};

}}

