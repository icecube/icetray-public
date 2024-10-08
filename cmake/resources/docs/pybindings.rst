.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. _writing-pybindings:

How to write pybindings for your project
========================================

.. highlight:: c++

#. Create a :file:`private/pybindings` directory in your project.
#. Write functions to instantiate Python proxies for your classes using `boost::python <https://www.boost.org/doc/libs/release/libs/python/>`_.
#. Write a module declaration that gathers your classes together in a common module.
#. Write a :file:`CMakeLists.txt` for your :file:`private/pybindings` directory.
#. Add the pybindings directory to your top-level :file:`CMakeLists.txt` with :code:`add_subdirectory('private/pybindings')`

Registering your class with boost::python
_________________________________________

Classes are registered by instantiating boost::python::class_<YourClass>.

::

	using namespace boost::python;

	scope particle_scope =
	  class_<I3Particle, bases<I3FrameObject>, boost::shared_ptr<I3Particle> >("I3Particle")
	  .def("GetTime", &I3Particle::GetTime)
	  .def("GetX", &I3Particle::GetX)
	  .def("GetY", &I3Particle::GetY)
	  .def("GetZ", &I3Particle::GetZ)
	;

Each of these methods returns the scope object, so they can be chained together.

Wrapping containers
___________________

Vectors of your types can by exposed with:

::

	class_<I3Vector<I3MyClass > >("I3MyClassSeries")
	  .def(vector_indexing_suite<I3Vector<I3MyClass > >())
	;

This will, among other things, make indexing and iteration work as expected for a Python object.

Maps can be exposed with:

::

	class_<I3Map<OMKey,I3Vector<I3MyClass > > >("I3MyClassSeriesMap")
	  .def(std_map_indexing_suite<I3Map<OMKey,I3Vector<I3MyClass > > >())
	;

In addition to indexing, this will make the wrapped map behave as much like a Python dictionary as possible.

Wrapping enumerated types
_________________________

Once you have exposed the methods of your class, you can expose enumerated types via :code:`boost::python::enum_<I3MyClass::MyEnumType>`.

::

    enum_<I3Particle::FitStatus>("FitStatus")
      .value("NotSet",I3Particle::NotSet)
      .value("OK",I3Particle::OK)
      .value("GeneralFailure",I3Particle::GeneralFailure)
      .value("InsufficientHits",I3Particle::InsufficientHits)
      .value("FailedToConverge",I3Particle::FailedToConverge)
      .value("MissingSeed",I3Particle::MissingSeed)
      .value("InsufficientQuality",I3Particle::InsufficientQuality)
      ;

Declaring the module
____________________

The wrapping code for each class should be placed in its own function::

	#include <dataclasses/physics/I3Particle.h>

	void register_I3Particle()
	{
	 {
	  boost::python::class_<I3Particle,
	                        bases<I3FrameObject>,
	                        boost::shared_ptr<I3Particle > >("I3Particle")
	  ...
	 }
	}

The Python module for the project is declared with the macro :c:macro:`I3_PYTHON_MODULE`. After loading your project's library, you can call the registration functions you defined for each class::

	I3_PYTHON_MODULE(dataclasses)
	{
	   load_project("dataclasses",false);
	   void register_I3Particle();
	   void register_I3Position();
	   void register_I3RecoPulse();
	}

If you named all your registration functions :code:`register_*`, you can use preprocessor macros to save some typing::

	#define REGISTER_THESE_THINGS (I3Particle)(I3Position)(I3RecoPulse)

	I3_PYTHON_MODULE(dataclasses)
	{
	   load_project("dataclasses",false);
	   BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
	}

Helpful preprocessor macros
___________________________

Writing pybindings can involve plenty of boilerplate code. Luckily, we include some macros that can be used with Boost preprocessor sequences to reduce the tedium.

Boost preprocessor sequences
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The header :code:`<boost/preprocessor/seq.hpp>` defines macros that can manipulate sequences. A sequence is a series of parenthesized tokens:

::

    #define MY_SEQUENCE (a)(whole)(bunch)(of)(tokens)

These tokens can be expanded with

.. c:macro:: BOOST_PP_SEQ_FOR_EACH(Macro, Data, Seq)

	Expand a sequence in place.

	:param Macro: A macro that takes three parameters: the head of the sequence, auxiliary data in Data, and an element of Seq.
	:param Data: Arbitrary data to be passed to every call of Macro.
	:param Seq: A sequence of tokens. Each of these tokens will be passed to Macro.

	Most of the macros mentioned here can be used with :c:macro:`BOOST_PP_SEQ_FOR_EACH` to automate repetitive declarations.

The following macros are defined in :file:`cmake/I3.h.in`:

Wrapping methods verbatim
^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:macro:: WRAP_DEF(R, Class, Fn)

	Method-wrapping macro suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent class of the member function
	:param Fn: Name of the member function

	This macro can be used to expose your interface to Python exactly as it is in C++::

		#define METHODS_TO_WRAP (GetTime)(GetX)(GetY)(GetZ)
		BOOST_PP_SEQ_FOR_EACH(WRAP_DEF, I3Particle, METHODS_TO_WRAP)

Since the Get/Set pattern is fairly common, there are iterable macros specifically for Get/Set. With these, one sequence can be used to define C++-style Get/Set methods and Python-style properties (see :c:macro:`WRAP_PROP`).

.. c:macro:: WRAP_GET(R, Class, Name)

	Define GetName(). Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. c:macro:: WRAP_GETSET(R, Class, Name)

	Define GetName() and SetName(). Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

	**Example**::

		#define NAMES_TO_WRAP (Time)(X)(Y)(Z)
		BOOST_PP_SEQ_FOR_EACH(WRAP_GETSET, I3Particle, NAMES_TO_WRAP)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, NAMES_TO_WRAP)

There are also versions of these macros (:c:macro:`WRAP_GET_INTERNAL_REFERENCE` and :c:macro:`WRAP_GETSET_INTERNAL_REFERENCE`) that return a reference rather than a copy.

Exposing private member data via Get/Set
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to be nice to your users, you can wrap your Get/Set methods in Python properties:

.. c:macro:: PROPERTY(Class, Prop, Fn)

	Add ``Class.Prop`` as a property with getter/setter functions :code:`GetFn()` / :code:`SetFn()`

	:param Class: Parent C++ class
	:param Prop: The name of the Python property
	:param Fn: The base name of the C++ Get/Set functions

.. c:macro:: PROPERTY_TYPE(Class, Prop, GotType, Fn)

	Add ``Class.Prop`` as a property with getter/setter functions :code:`GetFn()` / :code:`SetFn()`, specifying that :code:`GetFn()` returns ``GotType``. This is useful when wrapping overloaded getter functions.

	:param Class: Parent C++ class
	:param Prop: The name of the Python property
	:param GotType: The type returned by GetFn()
	:param Fn: The base name of the C++ Get/Set functions

.. c:macro:: WRAP_PROP(R, Class, Fn)

	Add ``Class.fn`` as a property with getter/setter functions :code:`GetFn()` / :code:`SetFn()`. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get/Set functions


.. c:macro:: WRAP_PROP_RO(R, Class, Fn)

	Add ``Class.fn`` as a property with getter function :code:`GetFn()`. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get function


	**Example**::

		#define DATA_TO_WRAP (Time)(X)(Y)(Z)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, DATA_TO_WRAP)

	Now in Python, I3Particle.x (yes, lowercase) will call and return I3Particle::GetX() and I3Particle.x = 0 will call I3Particle::SetX(0).

For finer-grained control of the Python property name, use the trinary form:

::

	PROPERTY(I3Particle, partyTime, Time)


Exposing public member data with access restrictions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can expose public member data as properties, either read/write or read-only:

.. c:macro:: WRAP_RW(R, Class, Member)

	Expose Member as a read/write Python property. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

.. c:macro:: WRAP_RO(R, Class, Member)

	Expose Member as a read-only Python property. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

	**Example**::

		#define MEMBERS_TO_WRAP (value)(some_other_value)
		BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3MyClass, MEMBERS_TO_WRAP)

Wrapping methods with call policies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you need finer-grained control of the return type of your wrapped methods, you can use the following macros:

.. c:macro:: GETSET(Objtype, GotType, Name)

	Define getter/setter methods to return by value.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods.

	For a name ``X``, this will define :code:`Objtype::GetX()` to return a ``GotType`` by value. This is appropriate for POD like ints and doubles. It will also define :code:`SetX()`.

.. c:macro:: GETSET_INTERNAL_REFERENCE(Objtype, GotType, Name)

	Define getter/setter methods to return by reference.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods.

	This will define :code:`Objtype::GetX()` to return a reference to ``GotType``, where ``GotType`` is still owned by the parent object. This is appropriate for compound objects like vectors and maps.

There are also trinary versions of these macros for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`:

.. c:macro:: WRAP_GET_INTERNAL_REFERENCE(R, Class, Name)

	Define :code:`GetName()` to return an internal reference. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. c:macro:: WRAP_GETSET_INTERNAL_REFERENCE(R, Class, Name)

	Define :code:`GetName()` and :code:`SetName()`. :code:`GetName()` will return an internal reference. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

.. c:macro:: WRAP_PROP_RO_INTERNAL_REFERENCE(R, Class, Fn)

	Add ``Class.fn`` as a property with getter function :code:`GetFn()`. :code:`GetFn()` will return a reference to the object owned by the C++ instance. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get function

.. c:macro:: WRAP_PROP_INTERNAL_REFERENCE(R, Class, Fn)

	Add ``Class.fn`` as a property with getter/setter functions :code:`GetFn()` / :code:`SetFn()`. :code:`GetFn()` will return a reference to the object owned by the C++ instance. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get/Set functions

Wrapping enumerated types
^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:macro:: WRAP_ENUM_VALUE(R, Class, Name)

	Add the value Name to an enumerated type. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The name of the C++ value.

	**Example**::

		enum_<I3Particle::FitStatus>("FitStatus")
		  #define FIT_STATUS (NotSet)(OK)(GeneralFailure)(InsufficientHits)      \
		                     (FailedToConverge)(MissingSpeed)(InsufficientQuality)
		  BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, I3Particle, FIT_STATUS)
		;

Constructing a Python module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:macro:: I3_REGISTER(r, data, t)

	For name ``Name``, call :code:`void register_Name()`. Suitable for use with :c:macro:`BOOST_PP_SEQ_FOR_EACH`.

	:param t: The suffix of the function name, e.g. register_t().
	:param data: unused.

.. c:macro:: I3_PYTHON_MODULE(module_name)

	Declare the following code to be run when the module is initialized.

	:param module_name: The name of the Python module. Must be a legal Python variable name.

	**Example**::

		#define REGISTER_THESE_THINGS (I3Particle)(I3Position)(I3RecoPulse)

		I3_PYTHON_MODULE(dataclasses)
		{
		   load_project("dataclasses",false);
		   BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
		}

Gotchas
_______

<unresolved overloaded function type> errors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You may be mystified by errors like these:

.. code-block:: none

	error: No match for 'boost::python::class_<
	    I3MCPMTResponse, boost::shared_ptr<I3MCPMTResponse>,
	    boost::python::detail::not_specified,
	    boost::python::detail::not_specified
	>::def(const char [11], <unresolved overloaded function type>)'

This can happen when the wrapped class exposes two different versions of the function, for example returning a const or non-const type. In this case, you have to specify the return type by hand. The :c:macro:`BOOST_PP_SEQ_FOR_EACH` tricks will not work; you'll need to use :c:macro:`GETSET` or :c:macro:`PROPERTY_TYPE` to wrap each name individually instead.

Naming conventions
__________________

Python properties are preferred over C++-style Get/Set methods. The exposed Python module should conform our :ref:`python-coding-standards` as closely as possible.

Resources
_________

- `Boost::Python wiki at python.org <https://wiki.python.org/moin/boost.python/HowTo>`_
- `Boost::Python reference guide <https://www.boost.org/doc/libs/release/libs/python/doc/reference/>`_

Todo: finer points of return-by-value vs. reference
___________________________________________________



