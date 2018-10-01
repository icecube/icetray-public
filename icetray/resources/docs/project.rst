How to make a project
=====================

CMake
-----

- A makefile generator
- Like autoconf, but better
    (not written in Modula-4)
- Uses it's own private language (bad, but not horrible)

Project Layout
--------------

::

  /
    CMakeLists.txt
    python/
        __init__.py
    private/
        modulename/
            Module.cxx
        pybindings/ (optional)
        converter/ (optional)
    public/ (optional)
        modulename/
             PublicInterface.h


IceCube cmake macros
--------------------

i3_project
^^^^^^^^^^

::

  i3_project(project_name
             DOCS_DIR resources/docs
             PYTHON_DIR python
            )

i3_add_library
^^^^^^^^^^^^^^

(First)      Name of library
(N args)     Names of files
USE_TOOLS    external tools
USE_PROJECTS icetray projects

:: 

  i3_add_library(lib_name
                 private/modulename/file1.cxx private/modulename/file2.cxx
                 USE_TOOLS boost gsl root
                 USE_PROJECTS icetray dataio dataclasses
                )

i3_add_pybindings
^^^^^^^^^^^^^^^^^

(First)      Name of project (should match directory name)
(N args)     Names of files
USE_TOOLS    external tools
USE_PROJECTS icetray projects

::

  i3_add_pybindings(project_name
                    private/pybindings/file1.cxx private/pybindings/file2.cxx
                    USE_TOOLS boost gsl root
                    USE_PROJECTS icetray dataio dataclasses
                   )

i3_test_scripts
^^^^^^^^^^^^^^^
This registers scripts with the test system and automatically runs them on the build bots.
Check out (http://builds.icecube.wisc.edu/) after every commit to see if you broke 
anything on other platforms.

It's common to include the following in your CMakeLists.txt file.  This will
automatically register every python script in 'resources/test'.
::

  i3_test_scripts(resources/test/*.py)
                  
Cmake syntax
------------

sh-like syntax:

- SET(VARIABLE_NAME "VALUE")
- message(STATUS "A status message: ${VARIABLE_NAME}")
- colormsg(CYAN " +- Project is not found.  Skipping")

Conditional Code
^^^^^^^^^^^^^^^^

If you can, optionally include parts of a module that require extra things.

Use AND and OR for boolean logic.

ex::

    if(SUITESPARSE_FOUND)
        # do things that require SuiteSparse
    else(SUITESPARSE_FOUND)
        colormsg(CYAN " +- SuiteSparse not found")
    endif(SUITESPARSE_FOUND)

Trivial Pybindings
------------------

If you don't have real pybindings, use this.

Note: don't mix this with real pybindings.

In python/__init__.py::

    form icecube.icetray imprt load
    load('project_name', False)
    del load

icetray.load's second argument controls printing status.  False disables printing
the load output status.

Documentation
-------------

.. note::

    Please document your project.  Besides being a good idea, it is a
    requirement for any serious project.

Doxygen documentation
^^^^^^^^^^^^^^^^^^^^^

Documents the C++ code.  Decent out of the box, but we can help it along.

Note: most comments are more appropriate for implementation files.
We'd actually like to read the header files instead of lines of comments.
(no one really reads the implementation files though).

The basic comment::

    /**
     * This is a function that finds the sqrt of a number
     */
    double Sqrt(double n) {
        return sqrt(n);
    }

Build the docs::

    make docs

This will also make sphinx docs.

Sphinx
^^^^^^

Documents the Python code.  Better styling.

Use restructured text (.rst).  Sphinx interprets this to make html.

Modify CMakeLists.txt::

    i3_project(advanced_bootcamp
        PYTHON_DIR python
        DOCS_DIR resources/docs
    )

Build only the Sphinx docs::

    make html

Section Syntax::

    # with overline, for parts
    * with overline, for chapters
    =, for sections
    -, for subsections
    ^, for subsubsections
    ", for paragraphs

http://sphinx-doc.org/rest.html


Implementation
--------------

Let's implement the basic framework of a module:

http://code.icecube.wisc.edu/svn/sandbox/advanced_bootcamp

Public Header
^^^^^^^^^^^^^

public/advanced_bootcamp/I3Bootcamp.h::

    // include guard
    #ifndef BOOTCAMP_H
    #define BOOTCAMP_H

    #include <icetray/I3FrameObject.h>
    #include <icetray/serialization.h>
    #include <string>

    // subclass I3FrameObject so we can insert this class into the frame
    class I3Bootcamp : public I3FrameObject {
    public:
        // interface goes here
        std::string text;
        int number;
        float number2;
    private:
        // basic boost serialization
        friend boost::serialization::access;
        template <typename Archive>
        void serialize(Archive &ar, unsigned version);
    };
    
    // icetray macro to make pointer typedefs for I3BootCampPtr, etc
    I3_POINTER_TYPEDEFS(I3Bootcamp);

    #endif // BOOTCAMP_H

Private Modules
^^^^^^^^^^^^^^^

private/advanced_bootcamp/I3Bootcamp.cxx::

    // the module interface
    #include <advanced_bootcamp/I3Bootcamp.h>
    
    // do serialization (write to/read from an i3 file)
    // the version number can be used to establish version formats
    template <typename Archive>
    void I3Bootcamp::serialize(Archive &ar, unsigned version)
    {
        // convince the serializer that we are an I3FrameObject
        // the & operator is both read and write
        // make_nvp (name,value pair) allows both binary and xml output
        ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
        
        // now actually serialize our contents
        // all default types and I3 types are serializable
        ar & make_nvp("text", text);
        ar & make_nvp("number", number);
        ar & make_nvp("number2", number2);
    }
    
    // another icetray macro to do most of the heavy lifting
    // for serialization
    I3_SERIALIZABLE(I3Bootcamp);

private/advanced_bootcamp/I3BootcampModule.cxx::

    // some basic includes
    #include <icetray/I3ConditionalModule.h>
    
    // the module interface
    #include <advanced_bootcamp/I3Bootcamp.h>
    
    // let's make a private module
    class I3BootcampModule : public I3ConditionalModule {
    public:
        // the constructor just calls the parent
        I3BootcampModule(const I3Context &ctx) : I3ConditionalModule(ctx) {}
        virtual ~I3BootcampModule() {}
        
        // process physics frames
        void Physics(I3FramePtr frame);
    }
    
    // use an icetray macro to make this work with icetray
    I3_MODULE(I3BootcampModule);
    
    void
    I3BootcampModule::Physics(I3FramePtr frame)
    {
        // make an I3Bootcamp object
        I3BootcampPtr output(new I3Bootcamp);
        output->number = 6;
        output->text = "Some text";

        // add the I3Bootcamp object to the frame
        frame->Put("BootcampStuff", output);
        // push the frame to the next module
        PushFrame(frame);
    }

Pybindings
^^^^^^^^^^

private/pybindings/module.cxx::
    
    #include <icetray/load_project.h>
    
    #include <public/advanced_bootcamp/I3Bootcamp.h>
    
    // register function for the interface class
    void register_I3Bootcamp()
    {
        // use an alias instead of "using boost::python"
        // saves us from really strange errors
        namespace bp = boost::python;
        
        // make a boost::python class
        // the I3BootcampPtr came from I3_POINTER_TYPEDEFS
        bp::class_<I3Bootcamp, I3BootcampPtr, bp::bases<I3FrameObject> >("I3Bootcamp")
            // make a python value (read/write)
            .def_readwrite("text",&I3Bootcamp::text)
            .def_readwrite("number",&I3Bootcamp::number)
            .def_readwrite("number2",&I3Bootcamp::number2)
        ;
    }
    
    // an icetray macro around the boost::python messiness
    I3_PYTHON_MODULE(advanced_bootcamp)
    {
        // load the c++ library
        // second argument is false to be quiet
        load_project("advanced_bootcamp", false);
        
        register_I3Bootcamp();
    }

python/__init__.py::

    # load the c++ pybindings
    from icecube.load_pybindings import load_pybindings
    load_pybindings(__name__, __path__)
    del load_pybindings
    
    
CMakeLists.txt
^^^^^^^^^^^^^^

code::
    
    i3_project(advanced_bootcamp)

    i3_add_library(advanced_bootcamp
            private/advanced_bootcamp/I3Bootcamp.cxx
            private/advanced_bootcamp/I3BootcampModule.cxx

            USE_TOOLS boost python
            USE_PROJECTS icetray dataclasses 
    )
    
    i3_add_pybindings(advanced_bootcamp
            private/pybindings/module.cxx
            
            USE_TOOLS boost python
            USE_PROJECTS icetray dataclasses
    )

Usage
^^^^^

Let's use this module to do something::

    from I3Tray import *
    from icecube import icetray, dataio, dataclasses, advanced_bootcamp

    tray = I3Tray()
    tray.AddModule('I3InfiniteSource', Stream=icetray.I3Frame.Physics)
    tray.AddModule('I3BootcampModule')
    
    def foo(frame):
        bootcamp = frame['BootcampStuff']
        print(bootcamp)
    tray.Add(foo)
    
    tray.AddModule('Dump')

    tray.AddModule('I3Writer', filename='foo.i3')

    tray.Execute()

    
