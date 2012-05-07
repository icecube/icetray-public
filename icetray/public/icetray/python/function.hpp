/*
 * Bare-bones pybindings for boost::function that can be called from both
 * Python and C++.
 *
 * From:
 * http://wiki.python.org/moin/boost.python/HowTo?action=AttachFile&do=get&target=py_boost_function.hpp
 * 
 *   AUTHOR: Alcides Viamontes Esquivel
 *
 *   LICENSE: You're free to use this file for whatever you want. You're 
 *            specially welcome to improve it in any way and give the 
 *            changes back.
 */

#ifndef ICETRAY_PYTHON_FUNCTION_HPP_INCLUDED
#define ICETRAY_PYTHON_FUNCTION_HPP_INCLUDED

#include <boost/python.hpp>
#include <boost/function.hpp>

#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

namespace boost { namespace python {

namespace detail {

template <typename RT, int arity>
struct pyobject_invoker;

template <typename RT>
struct pyobject_invoker<RT, 0>
{
	object callable;
	
	RT
	operator()()
	{
		return extract<RT>(callable());
	}
};

#define MAKE_PYOBJECT_INVOKER(z, n, data)                                  \
template <typename RT>                                                     \
struct pyobject_invoker<RT, n>                                             \
{                                                                          \
	object callable;                                                   \
	                                                                   \
	template < BOOST_PP_ENUM_PARAMS(n, typename Arg) >                 \
	RT operator()( BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg ) )         \
	{                                                                  \
		return extract<RT>(callable(BOOST_PP_ENUM_PARAMS(n, arg)));\
	}                                                                  \
};

BOOST_PP_REPEAT_FROM_TO(1, 6, MAKE_PYOBJECT_INVOKER, nothing);

#undef MAKE_PYOBJECT_INVOKER

template <typename Function>
::boost::shared_ptr<Function> function_from_object(object o)
{
	const int arity = Function::arity;
	typedef typename Function::result_type
	    result_type;
	pyobject_invoker<result_type, arity> invoker;
	invoker.callable = o;
	return ::boost::shared_ptr<Function>(new Function(invoker));
}

} // namespace boost::python::detail

template <typename Function>
class_<Function>
def_function(const char* name, const char* doc="")
{
	class_<Function> klass(name, doc, no_init);
	
	klass.def("__init__", make_constructor(&detail::function_from_object<Function>));
	klass.def("__call__", &Function::operator());
	
	return klass;
}

}} // namespace boost::python

#endif /* ICETRAY_PYTHON_FUNCTION_HPP_INCLUDED */
