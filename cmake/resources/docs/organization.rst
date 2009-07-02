Organization/Structure of Projects and Metaprojects
===================================================

IceCube software is divided up into projects, logical groupings of one
or more classes and modules that have similar or related
functions. Some examples are dataio, which contains modules that read
and write events, and linefit a project that contains one first-guess
reconstruction module. Projects work together to perform analysis and
simulation tasks.

Collections of projects are called meta-projects. Examples of
meta-projects are offline-software, icerec, and
simulation. Simulation, (perhaps obviously), contains projects needed
for simulation; other metaprojects perform specific reconstructions;
and there is a "stripped down" reconstruction metaproject containing
first-guess methods for filtering of data at the Pole itself.

.. _project-organization:

Project Directory Structure
---------------------------
IceCube projects have the following general directory structure:

.. code-block:: none

  .../src/
     |
     |-- A/ (CMakeLists.txt, .mailinglist, etc.)
     |       |
     |       |-- public/
     |       |     \-- A/  (*.h)
     |       |
     |       |-- private/
     |       |     |-- A/  (*.cxx)
     |       |     |-- pybindings/  (*.cxx)
     |       |     \-- test/         (main.cxx, test1.cxx...)
     |       |    
     |       |-- python/   (__init__.py + *.py) 
     |       |     
     |       \-- resources/
     |             |-- scripts/      (*.C, *.py)
     |             \-- docs/         (*.rst)
     |
     |-- B/ (CMakeLists.txt, .mailinglist, etc)
     |       |
     |       |-- public/
     |       |     \-- B/       (*.h)
     |       |
     |       |-- private/
     |       |     |-- B/  (*.cxx, ...)
     |       |     |-- pybindings/  (*.cxx)
     |       |     \-- test/         (main.cxx, test1.cxx...)
     |       |    
     |       |-- python/   (__init__.py + *.py) 
     |       |    
     |       \-- resources/
     |             |-- scripts/      (*.C, *.py)
     |             \-- docs/         (*.rst)
     |
   (etc)

A project is contained in a subdirectory of :envvar:`I3_SRC`
(:file:`.../src/` above). Projects are organized into private and
public sections, optionally with a test suite located in
``/private/test/``, subdirectories containing source that is to be
built into executables, and a resources directory with documentation,
scripts, and so on.

All the header files :file:`(*.h)` which describe the project's **public**
interface belong under the project's public/ subdirectory, in a
subdirectory with the same name as the project. Implementation files
(.cxx) and project-specific (nonpublic) header files (.h) belong under
the project's private/ subdirectory.

No other subdirectories are allowed under public/. The build system
will check this, (that is, for anything other than a directory with
the same name as the project itself) , and abort the build if such
dangerous file/directories exist. For example, if one creates the
subdirectory "dangerous" in project icetray's public directory, the
following appears::

  % make 
  Project "icetray" contains unsafe public files or directories:
  dangerous Build will fail momentarily.  This error will go away if
  you ensure that icetray contains only one directory under "public",
  named icetray.  This has been a recording.

Python and root scripts belong in :file:`my_project/resources/scripts`.

Documentation goes in :file:`resources/docs`, in files named
:file:`*.rst`, and in the source files themselves. We use doxygen for
automatically generated documentation and restructuredText for the
kind of documentation you're reading now.

Directories under private can be specified to be: 

  * code to be built and linked into the project's library 
  * source to be built into executables 
  * source to be built into executables and run as test suites
  * code to be built and linked into a python binding library.

See the reference and tutorial for more specifics.


Branches, Releases and The Trunk
--------------------------------

To aid in development of both projects and meta-projects, each have
three areas dedicated in the SVN repository, for example, in a project
called "superfit", you might find::

  $SVN/projects/superfit/trunk
  $SVN/projects/superfit/branches/pre-likelihood-conversion
  $SVN/projects/superfit/releases/V01-00-01

The "trunk" is a single development frontier, where releases are
usually made from, and the majority of development work is done,
especially for a smaller project.

The "branches" area is where special development work can take place.
For example, you might make a branch to try experimental work, allows
others to do some work in parallel to yours, or to make copies before
major changes are made to the trunk.

The "releases" area is where tagged, frozen copies of the trunk are
made for inclusion in software releases.  Releases should not be
changed once made, and most users do not have commit privledges in
this area.  Release are typically tagged with a number: VXX-YY-ZZ.

* XX - Major release number, should be incremented if the interface
  changes.

* YY - Minor release number, should be incremented if the interface is
  extended.

* ZZ - Bug fix version, incremented if a minor change is made fixing
  issues without changing the interface.

.. note:: 

   In fact, these conventions are almost universally ignored as they
   are difficult to test and enforce.  Generally one increments the X,
   Y, or Z based on some vague estimate of the size of the change.



