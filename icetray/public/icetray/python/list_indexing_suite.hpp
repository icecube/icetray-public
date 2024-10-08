//
// list_indexing_suite.hpp
// Copyright 2012 Jakob van Santen <vansanten@wisc.edu>
// Copyright 2012 The IceCube Collaboration <http://www.icecube.wisc.edu>
// SPDX-License-Identifier: BSL-1.0
//
// Adapted from vector_indexing_suite.hpp, (C) Copyright Joel de Guzman 2003,
// distributed under the terms of the Boost Software License.
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef ICETRAY_PYTHON_STD_LIST_INDEXING_SUITE_HPP_INCLUDED
#define ICETRAY_PYTHON_STD_LIST_INDEXING_SUITE_HPP_INCLUDED

#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/suite/indexing/container_utils.hpp>
#include <boost/python/iterator.hpp>
#include <boost/next_prior.hpp>
#include "pybindings.hpp"
#include <icetray/python/get_class.hpp>

#include <type_traits>

namespace boost { namespace python {

// Forward declaration
template <class Container, bool NoProxy, class DerivedPolicies>
class list_indexing_suite;

namespace detail
{
	template <class Container, bool NoProxy>
	class final_list_derived_policies
		: public list_indexing_suite<Container,
			NoProxy, final_list_derived_policies<Container, NoProxy> > {};

	template<class Container>
	static boost::shared_ptr<Container>
	container_from_object(boost::python::object v, typename std::enable_if<not std::is_arithmetic<typename Container::value_type>::value>::type* = 0)
	{
		boost::shared_ptr<Container> conti(new Container());
		container_utils::extend_container(*conti, v);
		return conti;
	}

	// For numeric vectors, instantiate this as container_from_object to
	// try using numpy's buffer protocol first
	template<class Container>
	static boost::shared_ptr<Container>
	container_from_object(boost::python::object v, typename std::enable_if<std::is_arithmetic<typename Container::value_type>::value>::type* = 0)
	{
		boost::shared_ptr<Container> x(new Container);
		Py_buffer view;
		if (PyObject_GetBuffer(v.ptr(), &view,
		    PyBUF_FORMAT | PyBUF_ANY_CONTIGUOUS) != -1) {
			if (strcmp(view.format, "d") == 0) {
				x->resize(view.len/sizeof(double));
				for (size_t i = 0; i < view.len/sizeof(double);
				    i++)
					(*x)[i] = ((double *)view.buf)[i];
			} else if (strcmp(view.format, "f") == 0) {
				x->resize(view.len/sizeof(float));
				for (size_t i = 0; i < view.len/sizeof(float);
				    i++)
					(*x)[i] = ((float *)view.buf)[i];
			} else if (strcmp(view.format, "n") == 0) {
				x->resize(view.len/sizeof(ssize_t));
				for (size_t i = 0; i < view.len/sizeof(ssize_t);
				    i++)
					(*x)[i] = ((ssize_t *)view.buf)[i];
			} else if (strcmp(view.format, "N") == 0) {
				x->resize(view.len/sizeof(size_t));
				for (size_t i = 0; i < view.len/sizeof(size_t);
				    i++)
					(*x)[i] = ((size_t *)view.buf)[i];
			} else if (strcmp(view.format, "i") == 0) {
				x->resize(view.len/sizeof(int));
				for (size_t i = 0; i < view.len/sizeof(int);
				    i++)
					(*x)[i] = ((int *)view.buf)[i];
			} else if (strcmp(view.format, "I") == 0) {
				x->resize(view.len/sizeof(int));
				for (size_t i = 0; i < view.len/sizeof(int);
				    i++)
					(*x)[i] = ((unsigned int *)view.buf)[i];
			} else if (strcmp(view.format, "l") == 0) {
				x->resize(view.len/sizeof(long));
				for (size_t i = 0; i < view.len/sizeof(long);
				    i++)
					(*x)[i] = ((long *)view.buf)[i];
			} else if (strcmp(view.format, "L") == 0) {
				x->resize(view.len/sizeof(long));
				for (size_t i = 0; i < view.len/sizeof(long);
				    i++)
					(*x)[i] = ((unsigned long *)view.buf)[i];
			} else if (strcmp(view.format, "q") == 0) {
				x->resize(view.len/sizeof(int64_t));
				for (size_t i = 0; i < view.len/sizeof(int64_t);
				    i++)
					(*x)[i] = ((int64_t *)view.buf)[i];
			} else if (strcmp(view.format, "Q") == 0) {
				x->resize(view.len/sizeof(uint64_t));
				for (size_t i = 0; i < view.len/sizeof(uint64_t);
				    i++)
					(*x)[i] = ((uint64_t *)view.buf)[i];
			} else {
				// We could add more types, but why do that?
				// Let Python do the work for obscure cases
				container_utils::extend_container(*x, v);
			}
			PyBuffer_Release(&view);
		} else {
			PyErr_Clear();
			container_utils::extend_container(*x, v);
		}

		return x;
	}
}

// list_indexing_suite is an indexing_suite derived class for
// wrapping both std::vector and std::list-like classes. Whereas
// std_vector_indexing_suite relies on the presence of operator[]
// and random-access iterators, list_indexing_suite uses boost::next
// to provide the same functionality with a proper fallback for
// containers with only forward iterators.

template <
	class Container,
	bool NoProxy = false,
	class DerivedPolicies
		= detail::final_list_derived_policies<Container, NoProxy> >
class list_indexing_suite
	: public indexing_suite<Container, DerivedPolicies, NoProxy>
{

public:
	typedef typename Container::value_type data_type;
	typedef typename Container::value_type value_type;
	typedef typename Container::size_type index_type;
	typedef typename Container::size_type size_type;
	typedef typename Container::difference_type difference_type;

	template <class Class>
	static void
	extension_def(Class& cl)
	{
		// container_from_object takes a boost::python object, but it must be
		// iterable and yield objects of value_type. generated an annotated
		// function signature for the docstring.
		{
			docstring_options doc_options;
			std::ostringstream oss;
			object klass = get_class<value_type>();
			oss <<  "__init__(self, container: typing.Iterable["
			    << (
					klass.is_none()
				    ? "typing.Any"
					: extract<std::string>(
						klass.attr("__module__") != "builtins" && klass.attr("__module__") != cl.attr("__module__")
						? "%s.%s" % make_tuple(klass.attr("__module__"), klass.attr("__qualname__"))
						: klass.attr("__qualname__")
					  )()
				)
				<< "]) -> None:"
			;
			doc_options.disable_py_signatures();
			cl.def("__init__", make_constructor(&container_from_object), oss.str().c_str());
		}
		cl
			.def("append", &base_append)
			.def("extend", &base_extend)
			.def("__value_type__", &get_value_type)
			.staticmethod("__value_type__")
		;
	}

	static boost::python::object get_value_type()
	{
		return get_class<value_type>();
	}

	static
	typename mpl::if_<
		is_class<data_type>,
		data_type&,
		data_type
		>::type
	get_item(Container& container, index_type i)
	{
		return *boost::next(container.begin(), i);
	}

	static object
	get_slice(Container& container, index_type from, index_type to)
	{
		if (from > to)
			return object(Container());
		return object(Container(boost::next(container.begin(),from),
			boost::next(container.begin(), to)));
	}

	static void
	set_item(Container& container, index_type i, data_type const& v)
	{
		*boost::next(container.begin(), i) = v;
	}

	static void
	set_slice(Container& container, index_type from, index_type to, data_type const& v)
	{
		if (from > to) {
			return;
		} else {
			container.erase(boost::next(container.begin(), from),
				boost::next(container.begin(), to));
			container.insert(boost::next(container.begin(), from), v);
		}
	}

	template <class Iter>
	static void
	set_slice(Container& container, index_type from,
		index_type to, Iter first, Iter last)
	{
		if (from > to) {
			container.insert(boost::next(container.begin(), from), first, last);
		} else {
			container.erase(boost::next(container.begin(), from),
				boost::next(container.begin(), to));
			container.insert(boost::next(container.begin(), from),
				first, last);
		}
	}

	static void
	delete_item(Container& container, index_type i)
	{
		container.erase(boost::next(container.begin(), i));
	}

	static void
	delete_slice(Container& container, index_type from, index_type to)
	{
		if (from > to) {
			return;
		} else {
			container.erase(boost::next(container.begin(), from),
				boost::next(container.begin(), to));
		}
	}

	static size_t
	size(Container& container)
	{
		return container.size();
	}

	static bool
	contains(Container& container, value_type const& v)
	{
		return std::find(container.begin(), container.end(), v)
			!= container.end();
	}

	static index_type
	get_min_index(Container& /*container*/)
	{
		return 0;
	}

	static index_type
	get_max_index(Container& container)
	{
		return container.size();
	}

	static bool
	compare_index(Container& /*container*/, index_type a, index_type b)
	{
		return a < b;
	}

	static index_type
	convert_index(Container& container, PyObject* i_)
	{
		extract<long> i(i_);
		if (i.check()) {
			long index = i();
			if (index < 0)
				index += DerivedPolicies::size(container);
			if (index >= long(container.size()) || index < 0) {
				PyErr_SetString(PyExc_IndexError, "Index out of range");
				throw_error_already_set();
			}
			return index;
		}
		PyErr_SetString(PyExc_TypeError, "Invalid index type");
		throw_error_already_set();
		return index_type();
	}

	static void
	append(Container& container, data_type const& v)
	{
		container.push_back(v);
	}

	template <class Iter>
	static void
	extend(Container& container, Iter first, Iter last)
	{
		container.insert(container.end(), first, last);
	}

private:

	static void
	base_append(Container& container, object v)
	{
		extract<data_type&> elem(v);
		// try if elem is an exact Data
		if (elem.check()) {
			DerivedPolicies::append(container, elem());
		} else {
		// try to convert elem to data_type
			extract<data_type> elem(v);
			if (elem.check()) {
				DerivedPolicies::append(container, elem());
			} else {
				PyErr_SetString(PyExc_TypeError,
					"Attempting to append an invalid type");
				throw_error_already_set();
			}
		}
	}

	static boost::shared_ptr<Container>
	container_from_object(boost::python::object v)
	{
		return detail::container_from_object<Container>(v);
	}

	static void
	base_extend(Container& container, object v)
	{
		std::vector<data_type> temp;
		container_utils::extend_container(temp, v);
		DerivedPolicies::extend(container, temp.begin(), temp.end());
	}
};

}} // namespace boost::python

#endif // ICETRAY_PYTHON_STD_LIST_INDEXING_SUITE_HPP_INCLUDED
