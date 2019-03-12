How to write pybindings for your project
========================================

#. Create a 'private/pybindings' directory in your project.
#. Write functions to instantiate Python proxies for your classes using boost::python.
#. Write a module declaration that gathers your classes together in a common module.
#. Write a CMakeLists.txt for your 'private/pybindings' directory.
#. Add the pybindings directory to your top-level CMakeLists.txt with add_subdirectory('private/pybindings')

Registering your class with boost::python
_________________________________________

Classes are registered by instantiating boost::python::class_<YourClass>. 

.. highlight:: c++

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

Once you have exposed the methods of your class, you can expose enumerated types via boost::python::enum_<I3MyClass::MyEnumType>.


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

The Python module for the project is declared with the macro c:func:`I3_PYTHON_MODULE`. After loading your project's library, you can call the registration functions you defined for each class::

	I3_PYTHON_MODULE(dataclasses)
	{
	   load_project("dataclasses",false);
	   void register_I3Particle();
	   void register_I3Position();
	   void register_I3RecoPulse();
	}

If you named all your registration functions register_*, you can use preprocessor macros to save some typing::

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

The header <boost/preprocessor/seq.hpp> defines macros that can manipulate sequences. A sequence is a series of parenthesized tokens:

::

    #define MY_SEQUENCE (a)(whole)(bunch)(of)(tokens)

These tokens can be expanded with 

.. c:function:: BOOST_PP_SEQ_FOR_EACH(Macro, Data, Seq)

	Expand a sequence in place.

	:param Macro: A macro that takes three parameters: the head of the sequence, auxiliary data in Data, and an element of Seq.
	:param Data: Arbitrary data to be passed to every call of Macro.
	:param Seq: A sequence of tokens. Each of these tokens will be passed to Macro.

	Most of the macros mentioned here can be used with c:func:`BOOST_PP_SEQ_FOR_EACH` to automate repetitive declarations.

The following macros are defined in cmake/I3.h.in:

Wrapping methods verbatim
^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: WRAP_DEF(R, Class, Fn)

	Method-wrapping macro suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent class of the member function
	:param Fn: Name of the member function

	This macro can be used to expose your interface to Python exactly as it is in C++ ::

		#define METHODS_TO_WRAP (GetTime)(GetX)(GetY)(GetZ)
		BOOST_PP_SEQ_FOR_EACH(WRAP_DEF, I3Particle, METHODS_TO_WRAP)

Since the Get/Set pattern is fairly common, there are iterable macros specifically for Get/Set. With these, one sequence can be used to define C++ style Get/Set methods and Python-style properties (see c:func:`WRAP_PROP`).

.. c:function:: WRAP_GET(R, Class, Name)

	Define GetName(). Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. c:function:: WRAP_GETSET(R, Class, Name)

	Define GetName() and SetName(). Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

	**Example**::

		#define NAMES_TO_WRAP (Time)(X)(Y)(Z)
		BOOST_PP_SEQ_FOR_EACH(WRAP_GETSET, I3Particle, NAMES_TO_WRAP)
		BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, NAMES_TO_WRAP)

There are also versions of these macros (c:func:`WRAP_GET_INTERNAL_REFERENCE` and c:func:`WRAP_GETSET_INTERNAL_REFERENCE`) that return a reference rather than a copy.

Exposing private member data via Get/Set
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to be nice to your users, you can wrap your Get/Set methods in Python properties:

.. c:function:: PROPERTY(Class, Prop, Fn)

	Add Class.Prop as a property with getter/setter functions GetFn()/SetFn()

	:param Class: Parent C++ class
	:param Prop: The name of the Python property
	:param Fn: The base name of the C++ Get/Set functions

.. c:function:: PROPERTY_TYPE(Class, Prop, GotType, Fn)

	Add Class.Prop as a property with getter/setter functions GetFn()/SetFn(), specifying that GetFn() returns GotType. This is useful when wrapping overloaded getter functions.

	:param Class: Parent C++ class
	:param Prop: The name of the Python property
	:param GotType: The type returned by GetFn() 
	:param Fn: The base name of the C++ Get/Set functions

.. c:function:: WRAP_PROP(R, Class, Fn)

	Add Class.fn as a property with getter/setter functions GetFn()/SetFn(). Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get/Set functions


.. c:function:: WRAP_PROP_RO(R, Class, Fn)

	Add Class.fn as a property with getter function GetFn(). Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

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

.. c:function:: WRAP_RW(R, Class, Member)
	
	Expose Member as a read/write Python property. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

.. c:function:: WRAP_RO(R, Class, Member)
	
	Expose Member as a read-only Python property. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

	**Example**::

		#define MEMBERS_TO_WRAP (value)(some_other_value)
		BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3MyClass, MEMBERS_TO_WRAP)

Wrapping methods with call policies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you need finer-grained control of the return type of your wrapped methods, you can use the following macros:

.. c:function:: GETSET(Objtype, GotType, Name)

	Define getter/setter methods to return by value.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods. 

	For a name X, this will define Objtype::GetX() to return a GotType by value. This is appropriate for POD like ints and doubles. It will also define SetX().

.. c:function:: GETSET_INTERNAL_REFERENCE(Objtype, GotType, Name)

	Define getter/setter methods to return by reference.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods. 

	This will define Objtype::GetX() to return a reference to GotType, where GotType is still owned by the parent object. This is appropriate for compound objects like vectors and maps.

There are also trinary versions of these macros for use with c:func:`BOOST_PP_SEQ_FOR_EACH`:

.. c:function:: WRAP_GET_INTERNAL_REFERENCE(R, Class, Name)

	Define GetName() to return an internal reference. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. c:function:: WRAP_GETSET_INTERNAL_REFERENCE(R, Class, Name)

	Define GetName() and SetName(). GetName() will return an internal reference. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

.. c:function:: WRAP_PROP_RO_INTERNAL_REFERENCE(R, Class, Fn)

	Add Class.fn as a property with getter function GetFn(). GetFn() will return a reference to the object owned by the C++ instance. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get function

.. c:function:: WRAP_PROP_INTERNAL_REFERENCE(R, Class, Fn)

	Add Class.fn as a property with getter/setter functions GetFn()/SetFn(). GetFn() will return a reference to the object owned by the C++ instance. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get/Set functions

Wrapping enumerated types
^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: WRAP_ENUM_VALUE(R, Class, Name)

	Add the value Name to an enumerated type. Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.

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

.. c:function:: I3_REGISTER(r, data, t)

	For name Name, call void register_Name(). Suitable for use with c:func:`BOOST_PP_SEQ_FOR_EACH`.  

	:param t: The suffix of the function name, e.g. register_t().
	:param data: unused.

.. c:function:: I3_PYTHON_MODULE(module_name)
	
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
	
This can happen when the wrapped class exposes two different versions of the function, for example returning a const or non-const type. In this case, you have to specify the return type by hand. The c:func:`BOOST_PP_SEQ_FOR_EACH` tricks will not work; you'll need to use c:func:`GETSET` or c:func:`PROPERTY_TYPE` to wrap each name individually instead.

Naming conventions
__________________

Python properties are preferred over C++-style Get/Set methods. The exposed Python module should conform to the `Style Guide for Python Code`_ as closely as possible.

.. _Style Guide for Python Code: http://www.python.org/dev/peps/pep-0008/


Resources
_________

- `Boost::Python wiki at python.org <http://wiki.python.org/moin/boost.python/HowTo>`_
- `Boost::Python reference guide <http://www.boost.org/doc/libs/1_57_0/libs/python/doc/v2/reference.html>`_

Todo: finer points of return-by-value vs. reference
___________________________________________________ 



