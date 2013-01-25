.. index:: MetaProjectTop
.. _MetaProjectTop:


Metaproject documentation
=========================

.. toctree::
   :maxdepth: 2

   metaproject/index

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

   projects/cmake/index
   projects/icetray/index
   projects/dataio/index
   projects/dataclasses/index
   projects/*/index

Of general interest
===================

.. toctree::
   :maxdepth: 2

   metaproject_metainfo
   code_standards
   netiquette
   gdb_python



Automatically Generated Documentation
=====================================

`Doxygen <doxygen/>`_

`Icetray-inspect <inspect/>`_


About this documentation
========================

See :ref:`Documentation`


..  FAQ

..  API Reference
..  -------------
.. `Epydoc-generated reference available here <../epydoc/index.html>`_


.. Indices and tables
   ------------------
	* :ref:`genindex`
	* :ref:`modindex`
..	* :ref:`search`



