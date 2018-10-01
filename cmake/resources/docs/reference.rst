.. _macroreference:

Icetray-specific cmake macro reference
======================================

See the cmake documentation for macros not listed here (not written by
us).

i3_project
----------

.. _i3_project:
.. index:: i3_project 
   single: CMake macros ; i3_project

.. cmake:: i3_project(project_name [ARGS])

   Define a project.

   **Example**::
     
      i3_project(fastreco
                 PYTHON_DIR python
                 DOCS_DIR   resources/docs
		 USE_SETUPTOOLS)

   **Targets created**:  none

   **Options**:

   .. cmdoption:: project_name
      
      The name of the project.  Don't use hyphens, though
      some projects do have hyphenated names.  Project names
      must start with a letter or underscore and contain
      only letters, digits 0-9 and underscores.

   .. index:: PYTHON_DIR
   .. cmdoption:: PYTHON_DIR <dir>

      The name of a directory containing python code that should be
      made available to the workspace. If *PYTHON_DEST* is not set,
      the *PYTHON_DIR* parameter causes a symbolic link to be made
      from ``$I3_BUILD/lib/icecube/``\ *project_name* to this project's
      *PYTHON_DIR*.  e.g. if I add a python file :file:`foo.py` to
      directory *X* in my project RZA, and pass *X* to
      :func:`i3_project`, I can access the code from my workspace via::

         from icecube.RZA import foo

      Note that, due to the way
      python packages work, these directories will need a file
      ``__init__.py`` in order to be importable.

      NB: If you use both pybindings and python code, you should code like
      this in your __init__.py::
	
	from icecube.load_pybindings import load_pybindings
	load_pybindings(__name__, __path__)

      See also :ref:`USE_SETUPTOOLS <USE_SETUPTOOLS>`

   .. _PYTHON_DEST:	   
   .. index:: PYTHON_DEST
   .. cmdoption:: PYTHON_DEST <dir>

      The name under which this directory should be available.  By
      default this is ``icecube/``\ *project_name*, so that the python
      is importable as::

        from icecube.project_name.somefile import function_or_class

      But in the case of projects with hyphenated names,
      e.g. ``phys-services``, *project_name* is not a legal python
      identifier and must be renamed.  In other cases this code should
      live at the toplevel directory, not under ``icecube``: then, you
      can add::

         PYTHON_DEST toplevelname

      and a link will be made from ``lib/toplevelname`` to
      ``project_name/PYTHON_DIR``, which means you can get at the code
      with::

         from toplevelname import <something>

      Note that if you wanted to, e.g. install the python for
      ``fast-reco`` at the python-legal name ``fast_reco``
      (transliterating hyphen to underscore), you need to include
      the leading ``icecube``::
 
         i3_project(fast-reco
           PYTHON_DIR python
           PYTHON_DEST icecube/fast_reco 
           )

   .. _DOCS_DIR:	   
   .. index:: DOCS_DIR 
   .. cmdoption:: DOCS_DIR <dir>


      Path to a directory containing restructuredtext documentation
      for html/latex generation with *Sphinx*       


   .. _USE_SETUPTOOLS:      
   .. index:: USE_SETUPTOOLS
   .. cmdoption:: USE_SETUPTOOLS

      Specifies that python *setuptools* should be used to setup and
      install python software in PYTHON_DIR.  PYTHON_DEST is ignored
      in this case.  

      At config time, a command::
     
        python setup.py develop

      will be run to create links in the build directory.  At install time
      a command::

        python setup.py install

      will be used to install the python into the tarball.

i3_add_library
--------------

.. _i3_add_library:

.. index:: i3_add_library 
   single: CMake macros ; i3_add_library

.. cmake:: i3_add_library(name src1 src2 ... srcN [ARGS ...])

   Add a library to the build.  

   **Example**::

     i3_add_library (dataio 
       private/dataio/*.cxx
       USE_TOOLS boost python 
       USE_PROJECTS icetray dataclasses interfaces
       )

   **Targets created**: a target called *name* that builds the library.

   **Options**:

   .. _USE_TOOLS:
   .. index:: USE_TOOLS
   .. cmdoption:: USE_TOOLS tool1 [tool2 tool3 ...]
   
      Add compile/link flags corresponding to the listed tools to the
      build of *name*.
   
   .. cmdoption:: USE_PROJECTS proj1 [proj2 ... projn]

      Add compile/link flags to the build of the current library for
      the listed projects.

   .. cmdoption:: ROOTCINT file1.h [file2.h ...]

      If a file LinkDef.h in the root of the project directory exists,
      attempt to generate and compile in a 'root' dictionary.
      
   .. cmdoption:: LINK_LIBRARIES lib1 [lib2 ... libn]

      Link in these additional libraries.

   .. cmdoption:: COMPILE_FLAGS flag1 [flag2 ...]

      Add these flags to the compile line.     

   .. cmdoption:: INSTALL_DESTINATION path

      On ``make install`` or ``make tarball``, install to this
      location within the install tree.  Path is relative to the
      installation prefix.

   .. cmdoption:: NOT_INSPECTABLE

      Don't run icetray-inspect on this library when generating
      documentation.  

   .. cmdoption:: WITHOUT_I3_HEADERS

      Don't prepend ``-include I3.h`` to compile lines.  (This is the
      global header that contains all the global hacks.

   .. cmdoption:: MODULE

      See cmake documentation of ``add_library``

   .. cmdoption:: EXCLUDE_FROM_ALL

      See cmake documentation of ``add_library``


i3_executable
-------------

.. index:: i3_executable 
   single: CMake macros ; i3_executable

.. cmake:: i3_executable(name src1 src2 ... srcN [ARGS ...])

   Compile and link *src1 ... srcN* together into binary *name*.

   **Example**::

     i3_executable(inspect 
       private/inspect/*.cxx
       USE_PROJECTS icetray
       USE_TOOLS boost python)

   **Targets created**:
   When called in a project named *PROJ*, creates a target
   *PROJ-name*.

   **Options**:

   .. cmdoption:: USE_TOOLS tool1 [tool2 tool3 ...]
      :noindex:
   
      As in i3_add_library.
   
   .. cmdoption:: USE_PROJECTS proj1 [proj2 ... projn]
      :noindex:

      As in i3_add_library.

   .. cmdoption:: LINK_LIBRARIES lib1 [lib2 ... libn]
      :noindex:

      As in i3_add_library.

   .. cmdoption:: NO_PREFIX

      Don't prepend project name to target generated, i.e.::

        i3_project(foo)
        i3_add_executable(bar main.cxx)

      would generate binary ``foo-bar``, whereas::

        i3_project(foo)
        i3_add_executable(bar main.cxx NO_PREFIX)

      would just generate target ``bar``

   .. cmdoption:: WITHOUT_I3_HEADERS
      :noindex:

      As in i3_add_library.
      

.. _i3_test_executable():

.. index:: i3_test_executable()
   single: CMake macros ; i3_test_executable()

.. cmake:: i3_test_executable(name src1 src2 ... srcN [ARGS ...])

   Add a unit-test executable to the build.

   **Example**::

     i3_test_executable(test 
       private/test/CleanConstructorTest.cxx  
       private/test/I3LoggingObjectFirst.cxx	    
       private/test/ServicesAtDestruction.cxx
       private/test/I3ConditionalModuleTest.cxx
       USE_TOOLS root
       USE_PROJECTS icetray)

   **Target created**: Same as ``i3_executable()``.  Does not create a
   target to run the tests after they are built: Scripts are run via
   the :ref:`testdriver`.


   **Options**:: This macro takes the same arguments as
   ``i3_executable()`` with the exception of ``NO_PREFIX``, which is
   not accepted.


   Add targets to build an executable as in ``i3_executable()``.
   Additionally, register the unit tests inside this binary for batch
   test running later: (For each file F in the list of *src* files,
   and for each unit test ''T'' registered with macro ``TEST()`` in
   these files, there will be a unit test created with name
   ``projectname/test/F.cxx/T``, e.g.
   ``icetray/test/I3FrameTest.cxx/deserialize_iterator`` or
   ``dataclasses/test/I3DOMStatusTest/class_versioning``.  See
   also :ref:`testdriver`


i3_add_pybindings
-----------------

.. index:: i3_add_pybindings 
   single: CMake macros ; i3_add_pybindings

.. cmake:: i3_add_pybindings(name src1 [src2 ... srcN] [ARGS ...])

   Add a python bindings library to the build.

   **Example**::

     i3_add_pybindings(icetray
       module.cxx
       OMKey.cxx
       I3Bool.cxx
       USE_TOOLS boost python 
       USE_PROJECTS icetray
       )

   **Targets created**:  A library target same as ``i3_add_library``, 
   but named *name-pybindings*.  Also certain compile flags are set
   and headers included to make it easier to build python bindings
   libraries:

   1.  The library generated is named
       :file:`$I3_BUILD/lib/icetray/mymodulename.so`, not
       :file:`$I3_BUILD/lib/libmymodulename.so`, so that it is importable by
       python like this:

       .. code-block:: python

	      from icecube import mymodulename

   #.  The additional compiler flag ``-DI3_PYBINDINGS_MODULE`` is set,
       which brings some functions (like
       ``register_pointer_conversions<T>()`` in to view.

   #.  ``Python.h`` is included first in every compilation unit.
       Python insists on this... otherwise there are various compile
       errors/warnings.

   **Options**:  Same as ``i3_add_library()``
       

i3_test_scripts
---------------

.. _i3_test_scripts():

.. index:: i3_test_scripts()
   single: CMake macros ; i3_test_scripts()

.. cmake:: i3_test_scripts(glob1 [glob2 ... globN])

   Register scripts to be run during testing.

   **Example**::

     i3_test_scripts(resources/scripts/*.py)

   **Targets created**: none. Scripts are run via the
   :ref:`testdriver`.
        
   **Options**: none
	
   Registers the scripts that match the glob expressions ``glob1
   .. globn`` as scripts to be run during testing.  For each script
   :file:`S.py` that matches one of the glob expressions, the test will be
   named ``projectname/scripts/S.py``, e.g.  ``I3Db/scripts/dumpdaq_rndflt.py`` or
   ``examples/scripts/pass1.py``.

      

qt4_i3_automoc
--------------

.. _qt4_i3_automoc():

.. index:: qt4_i3_automoc()
   single: Cmake macros ; qt4_i3_automoc()

.. cmake:: qt4_i3_automoc(file1 [file2 ... fileN])

   **Example**::

     qt4_i3_automoc( private/mygui/MyWidget.cpp )

   **Targets created**: none; .moc files will be written to the
   project's build directory.

   **Options**: none

   The moc tool will be run with the -DQT_NO_KEYWORDS option.  This
   means moc will ignore the keywords 'signals' and 'slots'; use instead
   the Q_SIGNALS and Q_SLOTS macros in your code.  This is done to ensure
   compatibility with boost's signals library.



