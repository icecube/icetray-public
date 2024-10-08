.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. _Documentation:

Documentation
=============

These docs were generated with Sphinx: https://www.sphinx-doc.org/

Requirements
------------

A recent version of sphinx (at least version 1.3) is required.  The
easiest way to install this version is::

   pip install --upgrade Sphinx

We use automatically-generated "doxyen" for C++ code-level
documentation and reStructuredText (rst, the standard for Python
documentation) for high-level and Python documentation.  You can click
the link "Show Source", on this page's side panel, to see what
reStructuredText looks like.

.. index:: Doxygen

.. _rst :
.. index:: reStructuredText

reStructuredText (rst)
----------------------

reStructuredText is the tool of choice for writing overview
documentation (and a good way to generate quick web pages).  See
:ref:`Links` below for source documentation.  Sphinx is a
documentation toolkit built on top of restructured text, used for
Python's standard documentation.

.. index:: DOCS_DIR

Projects that contain a :ref:`DOCS_DIR <DOCS_DIR>` in their
CMakeList.txt's :ref:`i3_project <i3_project>` invocation will have
documentation generated from the .rst source in that directory when
``make html`` or ``make docs`` is run.  For an example see the
documentation for project :ref:`dataio`. See also
:ref:`project-organization` and :ref:`workspace-targets`.

For project ``P``, the built html documentation will be found in
:ref:`$I3_BUILD <I3_BUILD>`:file:`/docs/projects/P`.

Doxygen
-------

Doxygen is a open-source application which generates good-looking
documentation of C, C++, Java and Python, from the source code
itself. One particularly appealing apspect of doxygen is its ability
to cross-reference classes and functions and generate inheritance and
collaboration diagrams. Try them out.

On the other hand, doxygen isn't very good for hand-written explanatory,
high-level documentation.

There is an excellent introduction on how to document the code located
at https://doxygen.nl/manual/docblocks.html


For a full description of what one can do with doxygen consult the
`Doxygen homepage <https://doxygen.nl>`_ , in particular the
`users manual <https://doxygen.nl/manual/>`_.

By default doxygen is configured to search the :file:`public/` and
:file:`resources/docs/` directories of your project looking for files
ending in the patterns ``.h``, ``.dox``, ``.cxx``, ``.cpp``, ``.C``,
and ``.cc``. The docs generated by putting doxygen-parsed comments
around your classes and methods generally give a good idea of what a
specific class does but, typically do not do a good job of explaining
how the classes fit together or how the project as a whole is to be
used. For this use reStructuredText.

Linking to doxygen docs from reStructuredText
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To provide a link over to the autogenerated doxygen documentation from
the rst docs, for project P, the path will be
``../../doxygen/P/index.html``.  e.g. for project dataio, the rst
source will look like::

  See the `doxygen <../../doxygen/dataio/index.html>`_ docs.


Building the documentation
--------------------------

Simply execute ``make docs`` in your project or the workspace.  See
also :ref:`workspace-targets`.


.. _Links:

Links
-----

* `Doxygen <https://www.doxygen.nl>`_
* `Sphinx  <https://www.sphinx-doc.org>`_
* `reStructuredText <https://docutils.sourceforge.io/rst.html>`_
* `reStructuredText Directives reference <https://docutils.sourceforge.io/docs/ref/rst/directives.html>`_
