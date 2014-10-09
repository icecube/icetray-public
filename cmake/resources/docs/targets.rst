.. _targets:
.. _workspace-targets:

Targets
=======

Workspace level
---------------

As the name suggests, workspace-level targets are available from the
workspace directory. These are a superset of the targets available in
project directories. The workspace-only targets perform higher level
tasks like creating projects and generating environment scripts. As
the table below explains, project-level targets invoked in the
workspace are forwarded to a child make processes in each project
directory.

.. tabularcolumns:: |l|l|

.. list-table:: Workspace targets
   :header-rows: 1
   :widths: 15, 85

   * - Target
     - Description
   * - help
     - Lists available targets
   * - rsync
     - Will rsync the latest test data down to your
       :envvar:`I3_TESTDATA` directory
   * - test
     - Will build everything, then run the test driver script to test 
       everything, then print a summary.    See :ref:`testdriver`
   * - test-bins
     - Builds test binaries. See :ref:`testdriver`
   * - pybindings
     - Builds python binding libraries.
   * - env-check
     - Checks your environment for corruption.  Used at various times
       in the build process to ensure that you haven't set
       :envvar:`ROOTSYS` in your :file:`.bashrc`.
   * - docs
     - make all of: *html*, *doxygen* and *inspect* 
   * - html
     - Generates this html documentation in :envvar:`I3_BUILD`/docs
   * - latex
     - Generates latex version of this documentation in :envvar:`I3_BUILD`/latex
   * - doxygen
     - runs doxygen to autogenerate documentation in :envvar:`I3_BUILD`/docs/doxygen
   * - inspect
     - builds :ref:`icetray-inspect` documentation in :envvar:`I3_BUILD`/docs/inspect
   * - rebuild_cache 
     - See documentation for cmake and :ref:`rebuild_cache`
   * - edit_cache 
     - See documentation for cmake and :ref:`rebuild_cache`

Project level
-------------

Project level targets include several standard targets like *clean*
and *all*, as well as whatever targets are created by calling 
cmake macros described in :ref:`macroreference` and in the 
standard cmake documentation.
		 




     
