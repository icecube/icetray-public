
.. _tooldetection:

Tool detection/configuration 
============================

In ``cmake/tools`` there are a collection of ``.cmake`` files that are
each responsible for detecting a tool and configuring the build to use
it correctly.  The rest of the cmake infrastructure 'uses' tool via 
several cmake variables.  For a tool *ZZZ*,

**ZZZ_INCLUDE_DIR**:

  The include directory that should be passed to the compiler via the
  ``-I`` flag.  This directory should normally contain one
  subdirectory with the name of the tool at the top of the path,
  unversioned, e.g.  ``boost_INCLUDE_DIR`` should equal
  ``/path/to/boost-1.36/include``, and that directory should contain
  only a subdirectory ``boost``, so that when reading code it is
  obvious which tool is responsible for what headers (eg::

    #include <boost/shared_ptr.hpp>

  And so that a change in version of a tool doesn't require rewriting
  any #includes in client code.   Note that::

    #include <shared_ptr.hpp>

  Which could come about if the INCLUDE_DIR were one level deeper, is
  incorrect, as it isn't obvious what tool that header file comes
  from.  (In the case of ``shared_ptr.hpp`` it isn't so bad, but try
  to track down *globals.h* in a workspace with ten or fifteen tools
  configured.  Not fun.)

**ZZZ_LIBRARIES**:

  A list of libraries to link to.  This is a semicolon-separated list
  of full paths to libraries, cmake-style, not just the names of the
  libraries.

**ZZZ_BIN_DIR**:

  Not often used but available if the tool provides executables for
  some reason.

**ZZZ_FOUND**:

  A flag needed by the rest of the build system to know that detection
  of **ZZZ** succeeded.

tooldef
-------

Note that there is a cmake macro *tooldef* which will automate a bit
of this.


.. _SYSTEM_PACKAGES:
 
SYSTEM_PACKAGES
---------------

.. index:: SYSTEM_PACKAGES

By default, tools are expected to be found on your system, and managed
by your system package manager. The major exception to this is if you
are running on one of the major clusters that supports CVMFS (UW
Madison, U. Maryland, DESY, etc). Tools there are managed in CVMFS,
transparently to the user.

.. _I3_SITE_CMAKE_DIR:

I3_SITE_CMAKE_DIR
-----------------

.. index:: I3_SITE_CMAKE_DIR
.. index:: /usr/share/fizzicks/cmake


Site specific handling of tools is supported as follows:

For each tool ``T``, the directory :file:`/usr/share/fizzicks/cmake`
(yes, that is really the path) is searched for ``T.cmake`` and if this
file is found, it is read in lieu of the standard tool definition
file.  This directory can be overridden by setting
``I3_SITE_CMAKE_DIR`` in your environment.

For instance, an override for tool *python* (filename
``python.cmake``) might look like::

  set(PYTHON_LIBRARIES "$ENV{HOME}/lib/libpython2.6.so" CACHE FILEPATH "Python library")
  set(PYTHON_INCLUDE_DIR "$ENV{HOME}/include/python2.6" CACHE PATH "Path the Python include directory")
  set(PYTHON_EXECUTABLE "$ENV{HOME}/bin/python" CACHE FILEPATH "Python interpreter")
  set(PYTHON_VERSION "2.6" CACHE STRING "Python version")
  set(PYTHON_FOUND TRUE CACHE BOOL "Python found")

Note the ``$ENV{HOME}`` sytax used in the example: since CMake doesn't
automatically mix in environment variables from the shell, you have to be explicit
when substituting values like ``HOME``.

.. note:: If you override *python* you **must** also test for *scipy*
          and *numpy*. See the `current tests
          <http://code.icecube.wisc.edu/projects/icecube/browser/IceTray/projects/cmake/trunk/tools/python.cmake>`_
          for an example.

.. warning:: If you use a nonsystem python as above, your toolset
   	     (specifically the boost_python component of the boost tool)
	     must be built against that same python.  See the page for
	     the :ref:`PythonTool` for more information.

Here is another example override for tools for *blas* and *lapack* (filenames
``blas.cmake`` and ``lapack.cmake``) used to find the
`ATLAS <http://math-atlas.sourceforge.net>`_ libraries on a Fedora 20 system::
        
        set(ATLAS_BASE "/usr/lib64/atlas")
        set(BLAS_FOUND TRUE CACHE BOOL "BLAS library found" FORCE)
        set(BLAS_LIBRARIES "${ATLAS_BASE}/libf77blas.so.3" CACHE FILEPATH "BLAS libraries" FORCE)
        set(LAPACK_FOUND TRUE CACHE BOOL "LAPACK library found" FORCE)
        set(LAPACK_LIBRARIES "${ATLAS_BASE}/liblapack.so.3"  CACHE FILEPATH "LAPACK libraries" FORCE)

Troubleshooting
---------------

.. index:: toolset ; troubleshooting
   
**Q**: Hello, I tried to install a new toolset and ran a problem at
compilation. It failed as follows::

   g++ -shared -Wl,-soname,libGX11.so.5.18 -m32 -O2
   -Wl,-rpath,/home/sabrina/work/new_tools/root-v5.18.00/lib -o
   lib/libGX11.so.5.18 x11/src/GX11Gui.o x11/src/Rotated.o
   x11/src/TGX11.o x11/src/gifdecode.o x11/src/gifencode.o
   x11/src/gifquantize.o x11/src/G__X11.o -lXpm -lXext -lX11 -lXft
   /usr/bin/ld: cannot find -lXft 
   collect2: ld returned 1 exit status

It seems that there is a missing "-lXft". Does someone have an idea what "lxft" is and where can I find it? 

**A**: That thing is a linker flag.  ``-l___`` means to find ``lib___.so``
and link it in.  Googling around for Xft and libXft reveals that this
is the X FreeType library.  This is a system package, you'll need to
install it to proceed.

If you are on ubuntu, the general procedure is to search though the
packages for the library name that is missing.   If you find one that
ends in ``-dev``, this one is likely to contain the missing
headers/libraries.
In this case there is indeed a package ``libxft-dev``.  Install it via
synaptic or the like.

On redhat or others, you're on your own.  You can try downloading and
installing tarballs manually....  good luck.


**Q**: We tried to install the tools on SL5.2 and the installer keeps
complaining about missing ``X11-devel`` headers, but the packages are
installed. does anybody know, how to solve this?  the error message
is: "Please install the X11 developer headers for your platform,
configure: error: Broken X11 install. No X11 headers", though the
package libX11-devel-1.0.3-9.el5 is installed

**A**: the header ``x11/intrinsic.h`` was missing, which is included in
``libXt-devel``

