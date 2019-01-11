.. index:: root
   pair:   tool; root
   pair:   root; Using odd libraries from
   single: ROOTSYS
   
.. highlight:: none


ROOT
====

The root tool detection script :file:`Findroot.cmake` sets the usual
cmake variables (see :ref:`tooldetection`) as well as
:envvar:`ROOTSYS`.

Using less common root components
---------------------------------

Root is not just one library, it is a *lot* of libraries.  You may
want to use a library from the ROOT framework that isn't added to the
link line by default when one simply adds *root* to :ref:`USE_TOOLS <USE_TOOLS>`.
For instance if you use something from root's *TMVA* package,
you might get runtime link errors like this::

  ***
  *** Failed to load library. (exceptions.RuntimeError)
  *** dlopen() dynamic loading error:
  /home/mdagost/icerec_V02-02-00/build/lib/libAtmCscdL3MLPFilter.so:
  undefined symbol: _ZN4TMVA6Reader7BookMVAERK7TStringS3_
  ***

If you run that garbled looking undefined symbol through *c++filt*,
you'll see::

  % c++filt 
  _ZN4TMVA6Reader7BookMVAERK7TStringS3_
  TMVA::Reader::BookMVA(TString const&, TString const&)

This symbol is inside root's ``libTMVA.so``.  To fix this problem you
will need to tell cmake to link libTMVA into your project's library
explicitly.  First you need to ask cmake to find the library for you.
In your project's CMakeLists.txt::

  find_library(ROOT_TMVA_LIB   # variable to set, containing the library
    TMVA                       # name of library to find, without 'lib' or '.so'
    PATHS ${ROOTSYS}/lib       # where to look. ROOTSYS set by root-detection script
    NO_DEFAULT_PATH            # in this case, only use the toolset ROOT 
    )

See the cmake documentation (try ``cmake --help-full | less``) for
``find_library`` for full details on this builting cmake macro.
``PATHS ${ROOTSYS}/lib`` means to look for the library named TMVA in
the directory ``${ROOTSYS}/lib``.  ``NO_DEFAULT_PATH`` means not to
search elsewhere (this is important...  you do not want to
accidentally mix root versions, this is the single biggest source of
frustration with root).  That invocation will set the cmake variable
``ROOT_TMVA_LIB`` to the path of the library, if found, else to
something like ``TMVA_NOTFOUND``.

Then tell :ref:`i3_add_library <i3_add_library>` to use this library::

  i3_add_library(myproject
    private/myproject/*.cxx
    USE_TOOLS  [ whatever tools you are using, including  'root' ]
    USE_PROJECTS [etc]
    LINK_LIBRARIES ${ROOT_TMVA_LIB}
    )

With this, *myproject* should be linked against root's *libTMVA.so*,
and the rest should happen automagically.

Having made :ref:`rebuild_cache`, you should see the setting for
``ROOT_TMVA_LIB`` in your CMakeCache.txt::

  ROOT_TMVA_LIB:PATH=/opt/i3/ports/root-v5.18.00/lib/libTMVA.so




