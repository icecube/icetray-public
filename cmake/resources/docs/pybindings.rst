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

Helpful preprocessor macros
___________________________

Writing pybindings can involve plenty of boilerplate code. Luckily, we include some macros that can be used with Boost preprocessor sequences to reduce the tedium.

The following macros are defined in cmake/I3.h.in:

Wrapping methods verbatim
^^^^^^^^^^^^^^^^^^^^^^^^^

.. cfunction:: WRAP_DEF(R, Class, Fn)

	Method-wrapping macro suitable for use with BOOST_PP_SEQ_FOR_EACH

	:param Class: Parent class of the member function
	:param Fn: Name of the member function

This macro can be used to expose your interface to Python exactly as it is in C++:
::

	#define METHODS_TO_WRAP (GetTime)(GetX)(GetY)(GetZ)
	BOOST_PP_SEQ_FOR_EACH(WRAP_DEF, I3Particle, METHODS_TO_WRAP)

Since the Get/Set pattern is fairly common, there are iterable macros specifically for Get/Set. With these, one sequence can be used to define C++ style Get/Set methods and Python-style properties (see :cfunc:`WRAP_PROP`).

.. cfunction:: WRAP_GET(R, Class, Name)

	Define GetName(). Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. cfunction:: WRAP_GETSET(R, Class, Name)

	Define GetName() and SetName(). Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

::

	#define NAMES_TO_WRAP (Time)(X)(Y)(Z)
	BOOST_PP_SEQ_FOR_EACH(WRAP_GETSET, I3Particle, NAMES_TO_WRAP)
	BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, NAMES_TO_WRAP)

There are also versions of these macros (:cfunc:`WRAP_GET_INTERNAL_REFERENCE` and :cfunc:`WRAP_GETSET_INTERNAL_REFERENCE`) that return a reference rather than a copy.

Exposing private member data via Get/Set
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to be nice to your users, you can wrap your Get/Set methods in Python properties:

.. cfunction:: PROPERTY(Class, Prop, Fn)

	Add Class.Prop as a property with getter/setter functions GetFn()/SetFn()

	:param Class: Parent C++ class
	:param Prop: The name of the Python property
	:param Fn: The base name of the C++ Get/Set functions

.. cfunction:: WRAP_PROP(R, Class, Fn)

	Add Class.fn as a property with getter/setter functions GetFn()/SetFn(). Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get/Set functions

.. cfunction:: WRAP_PROP_RO(R, Class, Fn)

	Add Class.fn as a property with getter function GetFn(). Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: Parent C++ class
	:param Fn: The name of the Python property and base name of the Get function

::

	#define DATA_TO_WRAP (Time)(X)(Y)(Z)
	BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, DATA_TO_WRAP)

Now in Python, I3Particle.x (yes, lowercase) will call and return I3Particle::GetX() and I3Particle.x = 0 will call I3Particle::SetX(0).

For finer-grained control of the Python property name, use the trinary form:

::

	PROPERTY(I3Particle, partyTime, Time)	


Exposing public member data with access restrictions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can expose public member data as properties, either read/write or read-only:

.. cfunction:: WRAP_RW(R, Class, Member)
	
	Expose Member as a read/write Python property. Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

.. cfunction:: WRAP_RO(R, Class, Member)
	
	Expose Member as a read-only Python property. Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: Parent C++ class
	:param Member: Name of public data member and Python property

::

	#define MEMBERS_TO_WRAP (value)(some_other_value)
	BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3MyClass, MEMBERS_TO_WRAP)

Wrapping methods with call policies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you need finer-grained control of the return type of your wrapped methods, you can use the following macros:

.. cfunction:: GETSET(Objtype, GotType, Name)

	Define getter/setter methods to return by value.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods. 

For a name X, this will define Objtype::GetX() to return a GotType by value. This is appropriate for POD like ints and doubles. It will also define SetX().

.. cfunction:: GETSET_INTERNAL_REFERENCE(Objtype, GotType, Name)

	Define getter/setter methods to return by reference.

	:param Objtype: The parent C++ class.
	:param GotType: The type of object returned by Get()
	:param Name: The base name of the Get/Set methods. 

This will define Objtype::GetX() to return a reference to GotType, where GotType is still owned by the parent object. This is appropriate for compound objects like vectors and maps.

There are also trinary versions of these macros for use with BOOST_PP_SEQ_FOR_EACH:

.. cfunction:: WRAP_GET_INTERNAL_REFERENCE(R, Class, Name)

	Define GetName() to return an internal reference. Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get method.

.. cfunction:: WRAP_GETSET_INTERNAL_REFERENCE(R, Class, Name)

	Define GetName() and SetName(). GetName() will return an internal reference. Suitable for use with BOOST_PP_SEQ_FOR_EACH.

	:param Class: The parent C++ class.
	:param Name: The base name of the Get/Set methods.

Todo: finer points of return-by-value vs. reference
___________________________________________________ 



