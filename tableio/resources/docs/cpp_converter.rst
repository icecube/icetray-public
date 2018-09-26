.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

Writing an I3Converter in C++
**********************************

.. highlight:: c++

You can also define your converters in C++. To do this, you need to inherit
from the templated C++ class I3ConverterImplementation<YourFrameObject>. The mechanics of
adding the converter to your project are more convoluted The following will demonstrate
how this in done for I3FilterResultMap from jebclasses.

CMakeLists.txt
___________________

.. highlight:: none

To use compiled converters, your project must link against tableio. You can
use the following conditional-compilation pattern in your CMakeLists file to
avoid a hard dependency on the tableio project::

    i3_project(jebclasses
      DOCS_DIR resources/docs)

    # define the lists used to build the basic library
    SET(LIB_${PROJECT_NAME}_SOURCEFILES private/jebclasses/*.cxx private/jebclasses/*/*.cxx)
    SET(LIB_${PROJECT_NAME}_PROJECTS icetray dataclasses)
    SET(LIB_${PROJECT_NAME}_TOOLS boost python)

    # add some things if hdf-writer is installed
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/tableio)
        LIST(APPEND LIB_${PROJECT_NAME}_PROJECTS tableio)
        LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES private/converter/*.cxx)
        ADD_DEFINITIONS(-DUSE_TABLEIO)
        MESSAGE(STATUS "     + tableio support")
    endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/tableio)

    # finally, define the library
    i3_add_library(${PROJECT_NAME} ${LIB_${PROJECT_NAME}_SOURCEFILES}
      USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
      USE_PROJECTS ${LIB_${PROJECT_NAME}_PROJECTS}
    )

The source files and dependencies related to tableio are only added if the directory build/tableio exists.

.. highlight:: c++

public/jebclasses/converter/I3FilterResultMap.h
__________________________________________________

You can declare your subclass in a header, like so::

    #include "tableio/internals/I3Converter.h"
    #include "jebclasses/I3FilterResult.h"
    
    class I3FilterResultMapConverter : public I3ConverterImplementation<I3FilterResultMap> {
        private:
            I3TableRowDescriptionPtr CreateDescription(const I3FilterResultMap& frmap);
            unsigned int FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows);
            bool magic_;
    };

Sometimes it is necessary to give your converter possible options (with
reasonable default values!). A good example are
the I3PositionConverter and I3DirectionConverter converters, which have the option
``BookRefFrame`` to specify in what reference frame the position and direction
data should be booked.

In C++ the option is defined as argument to the constructor method of the
converter. The header file of the I3PositionConverter gives a nice example on
this. It looks so::

    #include "tableio/I3Converter.h"
    #include "dataclasses/I3Position.h"

    class I3PositionConverter : public I3ConverterImplementation<I3Position>
    {
        public:
            enum BookRefFrame { car = 0, sph = 1, cyl = 2, all = 3 };
        
            I3PositionConverter()
            : BookRefFrame_(I3PositionConverter::car)
            {}
        
            /** The BookRefFrame argument specifies the reference frame for which
             *  position data should be booked.
             */
            I3PositionConverter(BookRefFrame BookRefFrame)
            : BookRefFrame_(BookRefFrame)
            {}
        
        private:
            I3TableRowDescriptionPtr CreateDescription(const I3Position& position);
            size_t FillRows(const I3Position& position, I3TableRowPtr row);

            BookRefFrame BookRefFrame_;
    };

The option ``BookRefFrame`` is an enum type in this case, but it can be any
type you want, e.g. ``int``, ``double``, or ``bool``.

If the user does not specify the option, the default constructor will be called
and it sets the ``BookRefFrame_`` class member variable to its default value
``I3PositionConverter::car``. Otherwise the option`ized constructor will be
called, which sets the ``BookRefFrame_`` class member variable to the enum
value, that the user has specified in his python script.

private/converter/I3FilterResultMap.cxx
__________________________________________

The implementation goes in a separate file::

    #include "jebclasses/converter/I3FilterResultMapConverter.h"

    I3TableRowDescriptionPtr I3FilterResultMapConverter::CreateDescription(const I3FilterResultMap& frmap) {
        I3TableRowDescriptionPtr desc = 
            I3TableRowDescriptionPtr(new I3TableRowDescription() );
        I3FilterResultMap::const_iterator it;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            desc->AddField<bool>(it->first,"bool","Field 0: condition passed, Field 1: prescale passed",2);
        }
        return desc;
    }
        
    unsigned int I3FilterResultMapConverter::FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows) {
        I3FilterResultMap::const_iterator it;
        bool* filter_result;
        for (it = frmap.begin(); it != frmap.end(); it++) {
            filter_result = rows->GetPointer<bool>(it->first);
            filter_result[0] = it->second.conditionPassed;
            filter_result[1] = it->second.prescalePassed;
        }
        return 1;
    }

Possible converter options (as class member variables) should be considered in
here ;) 

pybindings
__________________________________________

You also need trivial pybindings in order to register your converter for use
in tableio. There are preprocessor macros that automate most of this. You can
wrap the whole export block in #ifdefs to avoid hard dependencies. The content
of your project's converters pybindings file *private/pybindings/converters.cxx*
could look like so::

    #ifdef USE_TABLEIO
    
    #include "jebclasses/converter/I3FilterResultMapConverter.h"
    #include "tableio/converter/pybindings.h"
    
    void register_I3Converters() {
        I3CONVERTER_NAMESPACE(jebclasses);
        I3CONVERTER_EXPORT(I3FilterResultMapConverter,"Dumps the result of each Pole filter to a table column");
    }
    
    #else
    void register_I3Converters() {}
    #endif

The macro I3CONVERTER_NAMESPACE(jebclasses) sets up a Python module
icecube.jebclasses.converters into which pybindings for
I3FilterResultMapConverter are exported. Each :c:func:`I3CONVERTER_EXPORT` call
creates pybindings for the specified converter and registers it for use in
tableio.

pybindings for converters with options
------------------------------------------

If your converter defines possible options, the pybinding statements for each
converter become a bit more complicated because the option`ized C++ constructor
must be added my hand to the python converter class.

The pybindings for the I3PositionConverter shown above gives a nice example of
doing that.
The content of the ``register_I3Converters()`` function inside your pybindings
file *private/pybindings/converters.cxx* could now look like so (in case it
would be for I3PositionConverter, but which already exists as converter)::

    void register_I3Converters() {
        I3CONVERTER_NAMESPACE(dataclasses);
        I3CONVERTER_EXPORT(I3PositionConverter,
            "A nice (multiline) docstring for your converter \n"
            "describing all possible options goes in here!   \n"
        )
            .def(bp::init<I3PositionConverter::BookRefFrame>(bp::args("BookRefFrame")=I3PositionConverter::car));
        ;
    }

The syntax for the ``init`` method definition (the ``.def...`` line in the
example above) is::

    .def(bp::init<OPTION_1_TYPE, OPTION_2_TYPE, ...>(bp::args("OPTION_1_NAME") = OPTION_1_DEFAULT_VALUE, bp::args("OPTION_2_NAME") = OPTION_2_DEFAULT_VALUE, ...))

where *OPTION_1_TYPE*, *OPTION_2_TYPE*, ... are the C++ types of the option
values, *OPTION_1_NAME*, *OPTION_2_NAME*, ... are the names of the options
within Python, and *OPTION_1_DEFAULT_VALUE*, *OPTION_2_DEFAULT_VALUE*, ...
are the default values of the options, respectively.

**NOTE**:

    The code example above will not work, because the option *BookRefFrame*
    is of type enum and that enum must get pybindings, too. See the next section
    how to solve this!

Defining an enum inside the converter and using it as an converter option
-------------------------------------------------------------------------

It is quite common that an option should be an enum type option, as it is for
the I3PositionConverter. The enum should be placed inside the converter python
class in order to have a correct data structure within Python.

To place objects into the converter class, one needs to create the namespace of
the converter class. Than one can put the object (e.g. the enum) inside the
python class.

Three special macros exist to support the programmer doing this:

- I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ(converter, docstring)
- I3CONVERTER_CONVERTER_NAMESPACE(converter)
- I3CONVERTER_CONVERTER_OBJ(converter)

The ``I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ(converter, docstring)`` macro does
exactly the same thing as the ``I3CONVERTER_EXPORT(converter, docstring)`` macro
does, but it stores also the object of the converter class after its
registration in tableio into a variable. This variable can than be accessed
through the ``I3CONVERTER_CONVERTER_OBJ(converter)`` macro, e.g. to define an
additional method to it (in our case the option`ized C++ constructor method).

The ``I3CONVERTER_CONVERTER_NAMESPACE(converter)`` macro can be used to create
a namespace within the converter class.

To use enum type options, the enum has to get its pybindings BEFORE the
option`ized C++ constructor method is added to the class (via the .def
boost::python statement)!

The following code gives an example how to define the pybindings for the enum
first and than to add the option`ized C++ constructor method to the python
converter class::

    void register_I3Converters()
    {
        I3CONVERTER_NAMESPACE(dataclasses);
        
        I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ(I3PositionConverter,
            "A nice (multiline) docstring for your converter \n"
            "describing all possible options goes in here!   \n"
        );
        {
            I3CONVERTER_CONVERTER_NAMESPACE(I3PositionConverter);
            bp::enum_<I3PositionConverter::BookRefFrame>("BookRefFrame")
                .value("Car", I3PositionConverter::car)
                .value("Cyl", I3PositionConverter::cyl)
                .value("Sph", I3PositionConverter::sph)
                .value("All", I3PositionConverter::all)
                .export_values()
            ;
        }
        I3CONVERTER_CONVERTER_OBJ(I3PositionConverter)
            .def(bp::init<I3PositionConverter::BookRefFrame>(bp::args("BookRefFrame")=I3PositionConverter::car))
        ;
    }

The curly brackets around the converter namespace macro and the enum pybinding
are important! Because the namespace must be cleared before other converters can
be added to the project's converters namespace! Otherwise following defined
converters will end up in the current converter class!

