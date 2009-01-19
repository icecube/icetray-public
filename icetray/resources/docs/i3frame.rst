.. index:: 
   pair: I3Frame; C++ Class
.. _I3Frame:

.. highlight:: pycon

I3Frame
^^^^^^^

.. class:: icecube.icetray.I3Frame

   The ``I3Frame`` is the main container class for data in the icetray
   framework.  Data including but not limited to detector geometries,
   physics events, and calibrations are all stored in this flexible
   container.

   The ``I3Frame`` is essentially a smart C++ STL ``map`` of
   ``string`` to boost-shared-pointer-to-base, specifically
   ``boost::shared_ptr<I3FrameObject>``.

   .. method:: I3Frame()

      Create an I3Frame.  By default the stream will be *None*, or ``N``. 

   .. method:: I3Frame(stream)

      Create an I3Frame on stream *stream*::
      
        >>> frame = I3Frame(I3Frame.Physics)

      *stream* can also be a character, which is used to construct the *Stream* object::

        >>> frame = I3Frame('P')

   .. method:: Has(name)
   .. method:: __contains__(name)

      Returns a boolean indicating whether or not *name* is found in
      frame.  Note the double-underscore version which supports python's 'in' syntax::

        >>> from icecube import icetray
        >>> frame = icetray.I3Frame('x')
        >>> frame['foo'] = icetray.I3Int(6)
        >>> print frame
        [ I3Frame  (Frame type x):
          'foo' [Frame type x] ==> I3Int
        ]
        >>> frame.Has('foo')
        True
        >>> frame.Has('bar')
        False
	>>> 'bar' in frame
	False
	>>> 'foo' in frame
	True
       
   .. method:: Put(where, what)
   .. method:: __setitem__(where, what)

        Put object *what* at slot *where* in the frame.  This is
        also used by ``__setitem__``, so bracket-syntax works::

          >>> obj = icetray.I3Int(775)
          >>> frame.Put('some_int', obj)
          >>> obj2 = icetray.I3Int(767)
          >>> frame['other_int'] = obj2

        The object will be tagged with the frame's default stream.
        See the next form to specify the stream.

   .. method:: Put(where, what, stream)

        Put object *what* into frame at *where*, tagged with stream
        *stream*.  Note the subtleties of how streams are tagged::

           >>> frame = icetray.I3Frame('P')
	   >>> print frame
	   [ I3Frame  (Physics):
	   ]
	   #  new object ends up on the frame's default stream
	   >>> frame.Put('foo', icetray.I3Int(3))  
	   >>> print frame 
           [ I3Frame  (Physics):
             'foo' [Physics] ==> I3Int
           ]
           # or we can specify the stream
	   >>> frame.Put('bar', icetray.I3Int(3), icetray.I3Frame.Stream('G'))  
	   >>> print frame 
           [ I3Frame  (Physics):
             'foo' [Physics] ==> I3Int
             'bar' [Geometry] ==> I3Int
           ]

   .. method:: Get(where)
   .. method:: __getitem__(where)

        Get and return object from *where* in the frame.  If the frame
	was read from a file and this is the first attempt to Get the
	item in question, this may trigger the deserialization of the
	item::

           >>> obj = frame.Get('foo')
	   >>> print obj
	   <icecube.icetray.I3Int object at 0xb7d587d4>
	   >>> obj = frame['foo']
	   >>> obj
	   <icecube.icetray.I3Int object at 0xb7d587d4>

   .. method:: keys()

        Return a list of the frame's keys::

           >>> frame.keys()
           ['blah', 'foo']

   .. method:: values()

        Return a list of the frame's values::

           >>> frame.values()
           [<icecube.icetray.I3Int object at 0xb7d58824>, <icecube.icetray.I3Int object at 0xb7d587d4>]

   .. method:: items()

        Return a list of the frame's key/value pairs as 2-tuples::

           >>> frame.items()
	   [('blah', <icecube.icetray.I3Int object at 0xb7d58824>), 
            ('foo', <icecube.icetray.I3Int object at 0xb7d587d4>)]

   .. method:: size()
   .. method:: __len__()
 
        Return the number of entries in the frame::

          >>> frame.size()
          2
          >>> len(frame)
          2

        .. note:: Don't confuse this with the version of 'size' that takes a 
	   	  frame object key name.

   .. method:: size(name)

       Return the size of the buffer associated with frame object
       *name*.  If the frame has not been read from or written to disk
       this buffer may be empty.  Mostly useful for internal testing purposes::

         >>> frame.size('PoleIcLineFit')
         150

   .. method:: __str__()


        Pretty-print the frame to a string and return it::

          >>> print frame 
          [ I3Frame  (Physics):
            'CoincifyCombinedPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42274)
            'DrivingTime' [Physics] ==> I3Time (38)
            'I3EventHeader' [Physics] ==> I3EventHeader (91)
            'IceTopRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > > (46)
            'PoleCombinedLinefit' [Physics] ==> I3Particle (150)
            'PoleIcLinefit' [Physics] ==> I3Particle (150)
            'PoleTrackLlhFit' [Physics] ==> I3Particle (150)
            'PoleTrackLlhFit2' [Physics] ==> I3Particle (150)
            'TWCleanPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42562)
           ]
           
        On the first line is shown the stream that this frame is on.  Then for each frame item,

           '*keyname*\ ' [*stream*\ ] ==> *typename* (*bytes*\ ) 

        is shown, where *keyname* is the name the object is stored
        under, *stream* is which stream it is on, *typename* is what
        type it is, and *bytes* is the size of the buffer containing
        the serialized object (this might be zero if the object has
        not been loaded or stored, or if *drop_blobs* is on, in which
        case display of the size will be suppressed.
 	 
   .. method:: type_name(name)

      Return the type of the object named *name*::
 
          >>> frame.type_name('PoleIcLinefit')
          'I3Particle'

   .. method:: assign(otherframe)

      Erase all contents of the current frame, and assign the stream
      type and all contents of *otherframe* to *self*::

        >>> print frame
        [ I3Frame  (Physics):
          'CoincifyCombinedPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42274)
          'DrivingTime' [Physics] ==> I3Time (38)
          'I3EventHeader' [Physics] ==> I3EventHeader (91)
          'IceTopRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > > (46)
          'PoleCombinedLinefit' [Physics] ==> I3Particle (150)
          'PoleIcLinefit' [Physics] ==> I3Particle (150)
          'PoleTrackLlhFit' [Physics] ==> I3Particle (150)
          'PoleTrackLlhFit2' [Physics] ==> I3Particle (150)
          'TWCleanPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42562)
        ]
        >>> newframe = icetray.I3Frame()
        >>> print newframe
        [ I3Frame  (None):
        ]
        >>> newframe.assign(frame)
        >>> print newframe
        [ I3Frame  (Physics):
          'CoincifyCombinedPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42274)
          'DrivingTime' [Physics] ==> I3Time (38)
          'I3EventHeader' [Physics] ==> I3EventHeader (91)
          'IceTopRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > > (46)
          'PoleCombinedLinefit' [Physics] ==> I3Particle (150)
          'PoleIcLinefit' [Physics] ==> I3Particle (150)
          'PoleTrackLlhFit' [Physics] ==> I3Particle (150)
          'PoleTrackLlhFit2' [Physics] ==> I3Particle (150)
          'TWCleanPulses' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > > (42562)
        ]
        
   .. method:: clear()

      Delete all entries in the frame.

   .. method:: Delete(name)
   .. method:: __delitem__(name)

      Delete *name* from the frame.  the ``__delitem__`` version
      enables the more pythonic ``del`` style::

        >>> print frame
        [ I3Frame  (None):
          'foo' [None] ==> I3Int
          'bar' [None] ==> I3Int
        ]
        >>> frame.Delete('foo')
        >>> print frame
        [ I3Frame  (None):
          'bar' [None] ==> I3Int
         ]
        >>> del frame['bar']
        >>> print frame
        [ I3Frame  (None):
        ]

   .. method:: Rename(from, to)

      Move frame object at *from* to slot *to*::

         frame.Rename('foo', 'bar')

      This form **preserves** the stream tags, i.e. if 'foo', above,
      is on stream 'X', it will still be on 'X' when renamed to 'bar'.
      This is potentially much different than::

         frame['bar'] = frame['foo']
         del frame['foo']

      Since frame objects are tagged with the frame's stream belong to
      when they are *Put* into the frame, then the object at 'bar',
      above, will be tagged with whatever the frame's stream type is.
      Example::

          # Rename() preserves that the object is on the Geometry 
          # stream

          >>> print frame
          [ I3Frame  (Physics):
	    'bar' [Physics] ==> I3Int
	    'foo' [Geometry] ==> I3Int
          ]
          >>> frame.Rename('foo', 'quux')
          >>> print frame
          [ I3Frame  (Physics):
            'bar' [Physics] ==> I3Int
            'quux' [Geometry] ==> I3Int
          ]

          # note this one loses the original stream type
          >>> frame['foo'] = frame['quux']
          >>> del frame['quux']
          >>> print frame
          [ I3Frame  (Physics):
            'bar' [Physics] ==> I3Int
            'foo' [Physics] ==> I3Int
          ]


   .. method:: GetStop()

      Return the frame's stop::

        >>> frame.GetStop()
        >>> icetray.I3Frame.Physics

   .. method:: SetStop()

      Set the frame's stop::

        >>> frame.SetStop(icetray.I3Frame.Geometry)

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
