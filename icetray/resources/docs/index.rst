Icetray
=======

.. note::
   
   Herein, *module* has a couple of meanings.  One is ``I3Module``,
   which is the interface by which one writes plugins/extensions to
   icetray.  *Module* is also used in the sense of a group of python
   extensions, which, to add to the confusion, are more like icetray
   *projects* than icetray *modules*.  Were it possible at this stage
   I would rename the ``I3Module`` variety of module to 'plugin'.

   The use of *module* at the top right hand of this page (next to
   *index*), is in the python-module sense.  It is not supposed to
   take you to a listing of all I3Modules.


Table of Contents:

.. toctree::
   :maxdepth: 3

   classes
   logging
   framestreamstop
   inspect
   standard_modules
   scripting
   i3units
   newfeaturesv3
   changes

To Document
-----------

::

  % python -c 'import icecube.dataclasses'
  Traceback (most recent call last):
    File "<string>", line 1, in <module>
  RuntimeError: extension class wrapper for base class I3FrameObject has not been created yet


* The "lazy frame"
* Frame objects tagged with which stream they are on
* frame printout
