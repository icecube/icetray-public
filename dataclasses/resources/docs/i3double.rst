.. highlight:: pycon

:mod:`icecube.dataclasses` -- python bindings to dataclasses
============================================================

.. module:: icecube.dataclasses
   :synopsis: python bindings to dataclasses

.. class:: I3Double
   :noindex:

   Wrapper of the simple C++ class which holds a ``double`` called "value" 

   .. method:: I3Double()

      Create an I3Double with value initialized to 0.0

   .. method:: I3Double(v)

      Create an I3Double with value initialized to v

   .. attribute:: value
      :noindex:

      The 'double' that it holds

   Example::

      >>> d = dataclasses.I3Double()
      >>> d.value
      0.0
      >>> d = dataclasses.I3Double(7)
      >>> d
      <icecube.dataclasses.I3Double object at 0x2c9e940>
      >>> d.value
      7.0
      

