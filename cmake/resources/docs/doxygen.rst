Documenting your project with doxygen
=====================================

Doxygen is a open-source application which generates good-looking
documentation of c, c++, java and python, from the source code
itself. One particularly appealing apspect of doxygen is it's ability
to cross-reference classes and functions and generate inheritance and
collaboration diagrams. Try them out.

There is an excellent introduction on how to document the code located
here: http://doxygen.org/docblocks.html


For a full description of what one can do with doxygen consult the
`Doxygen homepage <http://doxygen.org>`_ , in particular the
`users manual <http://doxygen.org/manual.html>`_.

By default doxygen is configured to search the public/ and
resources/docs/ directories of your project looking for files ending
in the patterns .h, .dox, .cxx, .cpp, .C, and .cc. The docs generated
by putting doxygen-parsed comments around your classes and methods
generally give a good idea of what a specific class does but typically
do not do a good job of explaining how the classes fit together or how
the project as a whole is to be used. For this use restructuredText
(TODO: explain how)

Building the documentation
==========================
Simply execute make doxygen in your project or the workspace.

