.. index:: 
   pair: I3Frame; C++ Class
.. _I3Frame:

I3Frame
-------

The ``I3Frame`` is the main container class for data in the icetray
framework.  Data including but not limited to detector geometries,
physics events, and calibrations are all stored in this flexible 
container.

The ``I3Frame`` is essentially a smart C++ STL ``map`` of ``string``
to boost-shared-pointer-to-base, specifically
``boost::shared_ptr<I3FrameObject>``.

.. index:: I3FrameObject
.. _I3FrameObject:

To document
^^^^^^^^^^^

  * The "lazy frame"
  * Frame objects tagged with which stream they are on
  * frame printout

I3FrameObject
^^^^^^^^^^^^^

Types that are to be stored in an I3Frame must meet several requirements:

* Inherit publicly from ``I3FrameObject``
* Have a *serialization* method and associated instantiations
* define pointer-typedefs with I3_POINTER_TYPEDEFS

The struct ``I3Double`` in project ``dataclasses`` is a good example class
(this version is pared-down a bit.  Here is I3Double.h)::

  struct I3Int : public I3FrameObject     // inherits from I3FrameObject
  {
    int value;                            // this is the data it carries 
  
    template <typename Archive>
    void 
    serialize(Archive&, unsigned);        // here is the serialization method
  };
  
  I3_POINTER_TYPEDEFS(I3Int);             // this creates typedefs

Here is the implementation::

  #include <icetray/serialization.h>
  #include <dataclasses/I3Double.h>
  
  template <class Archive>
  void
  I3Double::serialize(Archive& ar,unsigned)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("value",value);
  }
  
  I3_SERIALIZABLE(I3Double);

See the documentation on :ref:`I3_SERIALIZABLE`,
:ref:`I3_POINTER_TYPEDEFS`, and the documentation for
boost::serialization for more details.  



.. index:: I3_POINTER_TYPEDEFS
.. _I3_POINTER_TYPEDEFS:

I3_POINTER_TYPEDEFS
-------------------

A call::

  I3_POINTER_TYPEDEFS(X);

Will expand to the following two typedefs::

  typedef boost::shared_ptr<X>        XPtr;
  typedef boost::shared_ptr<const X>  XConstPtr;

The invocation belongs in a header file after the definition of the
class ``X``.  By convention, all classes that are handled via
``shared_ptr`` should have one of these, and the typedefs should
be used in lieu of the expanded type.


.. index:: I3_SERIALIZABLE
.. _I3_SERIALIZABLE:

I3_SERIALIZABLE
---------------

The call::

  I3_SERIALIZABLE(ZZZ);

(found in file ZZZ.cxx in the private section of the project
containing class ZZZ) instantiates serialization routines for class
``ZZZ``.  You need this when ``ZZZ`` inherits from
:ref:`I3FrameObject` and has a serialization routine.
