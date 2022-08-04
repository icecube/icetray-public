.. _library_standards:

=================
Library Standards
=================

Introduction
============

These are standards and recommendations for developers of IceCube
software who wish to introduce a new "third party library" in to the
IceCube software stack. "Third party library" is defined as a library
that is developed and maintained by individuals or groups outside of
the IceCube collaboration. The following guidelines are not presented
in any particular order, but should be looked at as a group when
evaluating a new library.

Maintained
==========

The library must be maintained. Software with many unaddressed bug
reports or feature requests are not acceptable. The older the software
is, the more stringently this standard will be enforced.

Open Source
===========

The software must have an `OSI approved open source license
<https://opensource.org/licenses>`_ or be in
the public domain. Open source software with licensing or referencing
caveats need to be thoroughly examined and discussed, as these
caveats often invalidate open source licenses.

Documented
==========

The software must be thoroughly documented. "The docs are the source"
is not acceptable as many IceCube participants are not "coders".

Strong User Base
================

The software must have a solid user base. A solid as the software
may be, if it is only used by its author, it will quickly become unmaintained.

Ease Of Working With Developers
===============================

The developers must be easy to work with. Often times, IceCubers find
bugs in third party libraries. The authors of these libraries must be
responsive to bug reports and patches. It's not the responsibility of
IceCubers to maintain these external libraries, as it often takes time
and resources away from IceCube software and analysis.

Portability
===========

Portability is an obvious concern, since IceCube software is on many
different distributions of Linux and multiple versions of Mac OS X
(not to mention the the other operating systems used in edge
cases). At a minimum, a library must be supported by its developers on
the majority of the platforms our software currently runs on. See
:ref:`platforms` for a current list.

Portability is also an indication of the level of quality and
maintenance of a project.

Code Quality
============

The code of a third party library must be of high quality from both a
functional (it should work, and work correctly!) as well as a stylistic
standpoint. Unreadable code leads to broken code, which leads to
errors. Errors then lead to incorrect analysis.
