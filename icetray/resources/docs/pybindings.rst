Python Bindings (PyBindings)
============================

Allow python to import c++ libraries.


No Pybindings
------------------

If you don't have any pybindings, add the following to your python/__init__.py.  
This will allow your project to be loaded with a simple import in python.  If you
implement python bindings to your classes, skip to the next section.

python/__init__.py::

    from icecube.icetray import load
    load('<project_name>', False)
    del load

icetray.load's second argument controls printing status.  False disables printing
the load output status.


Pybindings
---------------

For more information check out the docs for `boost::python`_ (Version 1.58.0)

.. _boost::python: http://www.boost.org/doc/libs/1_58_0/libs/python/doc/tutorial/doc/html/index.html


private/pybindings/module.cxx::
    
    #include <icetray/load_project.h>
    
    #include <public/advanced_bootcamp/I3Bootcamp.h>
    #include <icetray/python/dataclass_suite.hpp>
    
    // write I3Bootcamp out to a string
    std::ostream& operator<<(std::ostream &s, const I3Bootcamp &thing)
    {
        s << "[I3Bootcamp text="<<thing.text<<" number="<<thing.number<<" number2="<<thing.number2<<"]";
        return s;
    }
    
    void setText(I3Bootcamp& thing, std::string& s)
    {
        thing.text = s;
    }
    std::string getText(const I3Bootcamp &thing)
    {
        return thing.text;
    }
    
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
            .def_readwrite("number",&I3Bootcamp::number)
            .def_readwrite("number2",&I3Bootcamp::number2)
            
            // make a python value (getter/setter)
            .add_property("text",&getText,&setText)
            
            // the advanced mode for if you have a lot of getters/setters
            // cmake/tool-patches/common/pybindings.hpp
            /*
            #define PROPS (Text)(Number)(Number2)
            BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Bootcamp, PROPS)
            #undef PROPS
            */
            
            // make a python function, with a docstring
            .def("getText",&getText, "Get the text")
            
            // add the dataclass_suite, which does string representation,
            // copying, pickling, operators, etc
            .def(bp::dataclass_suite<I3Bootcamp>())
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
    
    
Static Method
-------------

example::

    bp::class_<I3Bootcamp, I3BootcampPtr, bp::bases<I3FrameObject> >('I3Bootcamp')
        .staticmethod("SqrtNumber")
    ;

Operators
---------

example::

    bp::class_<I3Bootcamp, I3BootcampPtr, bp::bases<I3FrameObject> >('I3Bootcamp')
        .def(self ==  self)
        .def(self > self)
    ;

Abstract Base Classes
---------------------

Use an ABC to make a new python class.

The class::

    class I3BootcampService {
    public:
        ~I3BootcampService();
        virtual double DoStuff(int thing) = 0;
    }
    
The pybinding::

    class I3BootcampServiceWrapper : public I3BootcampService,
        bp::wrapper<I3BootcampService> {
        virtual double DoStuff(int thing) {
            return 0.;
        }
    }
