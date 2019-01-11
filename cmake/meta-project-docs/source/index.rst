
.. include:: metaproject/index.rst

.. this is here to provoke an early load of this module
.. apparently the rest of our projects should have a wrapper that loads
.. icecube.icetray itself.  If you don't do this you get the dreaded
..    File "/usr/lib/python2.5/site-packages/Sphinx-0.5dev_20080817-py2.5.egg/sphinx/ext/autodoc.py", line 400, in generate
..      todoc = module = __import__(mod, None, None, ['foo'])
..    RuntimeError: extension class wrapper for base class I3FrameObject has not been created yet


.. automodule:: icecube.icetray

Components of this metaproject
==============================

.. toctree::
   :glob:
   :maxdepth: 2

   projects/**/index

Of general interest
===================

.. toctree::
   :maxdepth: 2

   metaproject_metainfo
   netiquette
   gdb_python



Automatically Generated Documentation
=====================================

`Doxygen <doxygen>`_

`Icetray-inspect <inspect>`_


About this documentation
========================

These docs were generated with Sphinx: http://sphinx.pocoo.org/

A version of sphinx > 0.4.3 is required.  This means that as of this
writing, the trunk is required.     The easiest way to install this 
version is::

   easy_install http://svn.python.org/projects/doctools/trunk

Then see :ref:`workspace-targets` for information on building them.

Each project whose :ref:`i3_project() <i3_project>` macro invocation specifies a
:ref:`DOCS_DIR <DOCS_DIR>` will be linked to above.

   

..  FAQ

..  API Reference
..  -------------
.. `Epydoc-generated reference available here <../epydoc/index.html>`_


.. Indices and tables
   ------------------
	* :ref:`genindex`
	* :ref:`modindex`
..	* :ref:`search`



