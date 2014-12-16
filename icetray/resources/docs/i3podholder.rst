I3PODHolder
===========

This is a generic container for POD types - "plain old data" types.
The data is stored in a `value` parameter, and each class has the
standard comparison operators (`==, !=, <, >, <=, >=`) defined.

The instantiations of this container are::

    icetray.I3Bool
    icetray.I3Int
    dataclasses.I3Double
    dataclasses.I3String

For the `I3Int` and `I3Double` types, they will automatically compare
with normal `int` and `double` types in python, but will not automatically
convert to the type.

.. warning::

    Be careful comparing I3Double with double in python, as certain values
    will not compare equal because of the way floating point numbers are
    represented.
    
    For example::
    
        In [1]: dataclasses.I3Double(12.1) == 12.1
        Out[1]: False
    
    For more on floating point representation, see
    `Floating Point Arithmetic: Issues and Limitations <https://docs.python.org/2/tutorial/floatingpoint.html>`_.
