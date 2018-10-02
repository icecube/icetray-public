.. index:: 
   pair: I3Frame; C++ Class
.. _I3Frame:

.. highlight:: pycon


I3Frame
=========

The I3Frame is the fundamental container passed between I3Modules.


In C++
--------


I3Frame Reference
^^^^^^^^^^^^^^^^^

The I3Frame is a flexible container that carries data from I3Module 
to I3Module as an icetray run proceeds. I3Modules receive a shared 
pointer to an I3Frame when the framework calls their "stop" methods 
(Physics, DetectorStatus, Calibration, Geometry, etc.).

The I3Frame enforces a "write once, read many" policy: after an item 
has been placed in the frame, clients can examine that item, but not 
modify it. This is intended to catch the subtle and frustrating bugs 
that come up when foreign code modifies your data (intentionally or 
unintentionally) without telling you about it.

Internally, the I3Frame is implemented as a map<> of string to 
shared_ptr<I3FrameObject>. That is, only smart pointers to things that
inherit from I3FrameObject are storable in the frame. Any container such 
as I3Frame must require its contents to all have some common ancestor.

.. _i3frame_print:

If you send an I3Frame to cout (or 'print' it from python), it will
show you the names and types of its contents (this is what the
**Dump** I3Module does)::

  [ I3Frame  (Physics):
    'DrivingTime' [Physics] ==> I3Time (38)
    'InIceInitialData' [Physics] ==> I3Map<OMKey, I3RecoHitSeries> (2746)
    'LineFit' [Physics] ==> I3Particle (unk)
    'DipoleFit' [Physics] ==> I3Particle (unk)
    'PoleTriggers' [Physics] ==> I3Tree<I3Trigger> (298)
    'I3EventHeader' [Physics] ==> I3EventHeader (91)
    'Geometry' [Geometry] ==> I3Geometry (21042)
  ]
  
which follows the general scheme::

  [ I3Frame  (which_stream):
    'key0' [stream_0] ==> type0 (size0)
    'key1' [stream_1] ==> type1 (size1)
    ...
    'keyn' [stream_n] ==> typen (sizen)
  ]

where **which_stream** is the stream that the frame is 'on', (in
IceCube, this is *Physics*, *DAQ*, *DetectorStatus*, *Calibration*,
*Geometry*, or *TrayInfo*).  For each frame object, there is a line
that shows the key, which stream that object is on, the type of the
object, and the size, either an integer, or **unk**, meaning that the
object has not yet been serialized, so its size is not yet known.
(This is a bit of the *lazy frame* abstraction leaking through).

So above we can see that at 'LineFit' there is a pointer to an
I3Particle, and at 'Geometry' there is a pointer to an I3Geometry.

To put things into a frame, you call the frame's Put method. Here's 
a skeleton of the Physics() method of some module::

 void 
 MyModule::Physics(I3FramePtr frame)
 { 
   I3ParticlePtr my_particle(new I3Particle);  // make a new particle 
 
   // in here do something physics-y with the particle. 
 
   frame.Put("myparticle", my_particle);       // put it in the frame
 
   PushFrame(frame);                           // send the frame downstream
 }

That's really all there is to it. You call Put with a shared 
pointer to some thing, and a name specifying what slot this 
thing goes in. After this call, the frame from above would 
contain the new particle::

 [ I3Frame :
   'Calibration' ==> I3Calibration
   '
   'EventHeader' ==> I3EventHeader
   'Geometry' ==> I3Geometry
   'InIceRawData' ==> I3Map<OMKey, I3Vector<I3DOMLaunch> >
   'InIceRecoHitSeries' ==> I3Map<OMKey, I3Vector<I3RecoHit> >
   'myparticle' ==> I3Particle   // (here it is!)
   'simpleseed' ==> I3Particle
 ]

Getting that particle back out, say in the Physics() method of 
a module later in the processing chain, looks like this::

 void 
 OtherModule::Physics(I3FramePtr frame)
 { 
   // get a const shared_ptr
   I3ParticleConstPtr the_particleptr = 
      frame.Get<I3ParticleConstPtr>("myparticle");
   
   PushFrame(frame); // send the frame downstream
 }

Here we've got a const shared pointer to the I3Particle. This is ideal: you
can check that you got what you asked for (check for non-null pointer) and 
the_particleptr is const, so the compiler can ensure we don't
accidentally modify it.

Requirements on toplevel frame objects
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Toplevel frame objects... that is, those which you will directly Put
and Get to/from the frame must satisfy several requirements:

 * Be descendants of I3FrameObject
 * Have at least one virtual, non-inline function (an empty virtual
   destructor, declared in the header file but defined in the
   implementation file, will do if the class has no other need for
   virtual functions)
 * Have a serialize method that also serializes its base classes via
   base_object
 * Use the I3_SERIALIZABLE() macro to instantiate the serialization
   method for the relevant archive types.

These requirements allow toplevel frame objects to work correctly with
the frame's Get<> methods and to function correctly in the I3Writer,
I3Reader, and other applications like the dataio-shovel.

I3Map<> and I3Vector<> exist strictly to help std::vector and std::map
satisfy the requirements above, so that these maps/vectors are
storable as toplevel frame objects.

maps and vectors that are contained in toplevel frame objects, but are
not themselves toplevel frame objects, need not be I3Maps or
I3Vectors; plain vector and map will do.

I3Bool and I3Double are two other good examples of classes that exist
only to contain data such that they are storable in the frame.

I3Frame::Put()
^^^^^^^^^^^^^^

Put() will succeed if no object exists in the frame at the requested
key. If something already does, Put() throws an exception via a call
to log_fatal(). For instance, this code::

 I3ParticlePtr p(new I3Particle);
 frame.Put("myparticle", p);
 frame.Put("myparticle", p); // this one is in error.  Slot full already.

generates the error::

 Frame already contains myparticle of type I3Particle

Notice that this function works only for shared_ptr<T> where T is or
inherits from I3FrameObject. One cannot, for instance, place a string
directly into the frame::

 std::string datum = generate_datum();                   
 frame.Put(datum, "datum_key");    // error, datum not a shared_ptr

the error is returned::

 badput.cxx:23: error: no matching function for call to 'I3Frame::Put(const
  char [10], std::string&)'
 public/icetray/I3Frame.h:164: note: candidates are: void I3Frame::Put(cons
 t std::string&, boost::shared_ptr<const I3FrameObject>)

Because datum is not a shared_ptr. Again, not just any shared_ptr will
work. If we attempt to pass a shared_ptr to a string::

 shared_ptr<string> datum_p(new string);
 \*datum_p = generate_datum();
 frame.Put(datum_p, "datum_key"); // error, string not a I3FrameObject

string is not a descendant of I3FrameObject, which is a requirement
placed on all frame objects. The error is a little longer::

 /icecube/work/offline-mk/tool-patches/boost/shared_ptr.hpp: In constructor
  'boost::shared_ptr<T>::shared_ptr(const boost::shared_ptr<Y>&) [with Y =
 std::string, T = const I3FrameObject]':
 badput.cxx:23:   instantiated from here
 /icecube/work/offline-mk/tool-patches/boost/shared_ptr.hpp:186: error: can
 not convert 'std::string* const' to 'const I3FrameObject*' in initialization

Where the last line is the most important: can't convert from
pointer-to-string to pointer-to-I3FrameObject. It's easy enough to
fix::

 struct MyStuff : public I3FrameObject
 {
   string value;
 };
 
 I3_POINTER_TYPEDEFS(MyStuff);

 MyStuffPtr mystuff_p(new MyStuff);             // create a MyStuff
 mystuff_p->value = "badgerbadgerbadgerbadger"; // set some value
 frame.Put(mystuff_p, "snaaaake");              // put in frame

Put with default names
^^^^^^^^^^^^^^^^^^^^^^

There are actually two forms of ``I3Frame::Put()``. One variation is
for objects without associated default names (that's almost all of
them), as used above in ``MyModule::Physics()``, ``void Put(const
string& name, shared_ptr<I3FrameObject> element)``; and the other
omits the name parameter::

 template <typename T>
 void 
 Put(shared_ptr<T> element);

If the object has a default name (though putting default-named objects
into the frame is a job that only authors of specific, specialized
modules will do), Put will work like this::

 I3GeometryPtr geo(new I3Geometry);
 frame.Put(geo);

I3Frame::Get<T>()
^^^^^^^^^^^^^^^^^

This function:

 * Verifies that some object exists at the location requested
 * Verifies that the object there is actually of the type requested
 * Returns a const reference or const shared_ptr to the object,
   depending on the arguments passed.

Get takes two forms. One returns a const reference to an object, the
other returns a shared_ptr to a const object.

Getting a SomethingConstPtr
^^^^^^^^^^^^^^^^^^^^^^^^^^^

You generally want to get objects from the frame as shared pointer.

This form of Get retrieves a shared_ptr to a frame object::

 template <typename T>
 T
 Get(const std::string& key);

The frame will first attempt to locate an object
at key, and an object does exist there, the frame will then attempt to
dynamic_pointer_cast this object to the template argument T. If either
of these steps fails, the frame returns a null TPtr (or
shared_ptr<const > if you like).

Note that this function will only compile if the type T is const. That
is::

 I3ParticlePtr particle = frame.Get<I3ParticlePtr>("linefit_result");


will not compile. The reason for this is that what you're requesting,
above, is not a const pointer: the module executing the code above
would be able to change a frame item that it had not put there. Your
collaborators agree almost unanimously that to allow this is a Bad
Idea. The code above is easy enough to fix, though::

 I3ParticleConstPtr particle = frame.Get<I3ParticleConstPtr>("linefit_result");
           

This works fine. Note that I3ParticleConstPtr is a typedef of
shared_ptr<const I3Particle>. See :ref:`I3_POINTER_TYPEDEFS`.

Getting a reference
^^^^^^^^^^^^^^^^^^^

There are times when getting a reference is preferred, for example,
when you want a fatal error when objects that absolutely should be present
are somehow missing.

::

 template <typename T>
 const T& 
 I3Frame::Get(const std::string& key);

This version returns a const reference to the item at location key in
the frame, if something exists at that name and a dynamic_cast of that
item to type T succeeds. It follows that one can retrieve object O as
type T from the frame if and only if O is of type T or a type derived
from T.

If either of these two preconditions fail, the I3Frame will throw an
exception of type std::runtime_error.

Object in the frame may have default names: see I3_DEFAULT_NAME
. Objects who have some default name defined (like I3Geometry) do not
require a string key argument. Less 'stable' classes like I3Particle
or I3Hit do not have default locations, in these cases one must pass
I3Frame::Get a key name.

Get a const reference to the geometry::

 const I3Geometry& geometry = frame->Get<I3Geometry>();

(Notice that I3Geometry doesn't require a "key" argument.

Get a const reference to an I3Particle placed there by some module
earlier in the module chain::

 const I3Particle& seed = frame->Get<I3Particle>("linefit_result");

This one does require a specific key name. ("linefit_result", above.)


Examples
^^^^^^^^

Some more examples. Get a shared_ptr to the geometry::

 void 
 I3LineFit::Physics(I3FramePtr frame)
 { 
   I3GeometryConstPtr geo_p = frame->Get<I3GeometryConstPtr>();
   ...

Get a const shared_ptr to some I3Particle::

 I3ParticleConstPtr seed_p = frame->Get<I3ParticleConstPtr>("linefit_result");

Given classes Base and Derived, one can retrieve an object of class
Derived as either::

 class Base : public I3FrameObject { ... };
 class Derived : public Base { ... };
 
 DerivedPtr derived(new Derived);
 frame.Put("something_derived", derived);
 
 BasePtr base(new Base);
 frame.Put("something_base", base);
 
 // ok.
 const Derived& d_ref = frame.Get<Derived>("something_derived");  
 
 // ok.  Derived inherits from Base.
 const Base& b_ref = frame.Get<Base>("something_derived");       
 
 // throws runtime_error:  this Base won't dynamic_cast to Derived
 const Derived& d_ref2 = frame.Get<Derived>("something_base");   
 
 // ok.  
 const Base& b_ref = frame.Get<Base>("something_base");       


I3Frame::Rename(const string& from, const string& to)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This works as its name suggests, it renames a I3FrameObject in the
frame.

Preconditions: slot "from" in frame contains something, and slot "to"
is empty

Postconditions: slot "from" in frame is empty, and "to" contains that
something.

I3Frame::Delete(const string& where)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Also works as its name suggests, deletes a I3FrameObject in the frame.

Preconditions: slot "where" in frame contains something.

Postconditions: slot "where" in frame is empty. 

Frame names may not contain whitespace
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The name associated with the data in the frame has one restriction: it
may not contain whitespace. The dataio modules and various utilities
available for splicing and filtering data streams use whitespace to
separate lists of frame names that they should keep or ignore. If you
attempt such a thing, for instance ::

 frame.Put("bogus name",something) 

you will see something like::

 public/icetray/I3Frame.h:115: Attempt to Put() element into frame at name
 "bogus name", which contains whitespace.

.. index:: I3_POINTER_TYPEDEFS
.. _I3_POINTER_TYPEDEFS:

I3_POINTER_TYPEDEFS
^^^^^^^^^^^^^^^^^^^

The macro invocation ``I3_POINTER_TYPEDEFS(T)``  expands to::

 typedef shared_ptr<T> TPtr;
 typedef shared_ptr<const T> TConstPtr;

Notice the second typedef. This is useful when getting things out of
the frame. It is not sufficient to prepend the first typedef (TPtr,
above) with const, like this::

 const I3ParticlePtr particle = frame.Get<I3ParticlePtr>("seed");

which is equivalent to::

 const shared_ptr<I3Particle> particle = 
   frame.Get<shared_ptr<I3Particle> >("seed");

as this const I3ParticlePtr is a const pointer to a nonconst particle;
the pointer isn't changeable, and the particle is. This is of course
the opposite of the desired effect, and by design, the line above
won't compile. The second typedef, TConstPtr, is the correct one::

 I3ParticleConstPtr particle = frame.Get<I3ParticleConstPtr>("seed");

Wherein the pointer to the particle is mutable, but the particle
itself is not. If you look at the code with the typedefs lifted, you
can see that the const is associated with the particle, not the
pointer::

 shared_ptr<const I3Particle> particle = 
   frame.Get<shared_ptr<const I3Particle> >("seed");

If you attempt to Get<> something non-const, you will see an error
something like::

 /Users/troy/Icecube/meta-projects/offline-software/work-dc2/offline-mk/too
 l-patches/boost/shared_ptr.hpp: In constructor 'boost::shared_ptr<T>::shar
 ed_ptr(const boost::shared_ptr<Y>&, boost::detail::dynamic_cast_tag) [with
  Y = const I3FrameObject, T = TestedData]':
 /Users/troy/Icecube/meta-projects/offline-software/work-dc2/offline-mk/too
 l-patches/boost/shared_ptr.hpp:416:   instantiated from 'boost::shared_ptr
 <T> boost::dynamic_pointer_cast(const boost::shared_ptr<U>&) [with T = Tes
 tedData, U = const I3FrameObject]'
 public/icetray/I3Frame.h:84:   instantiated from 'boost::shared_ptr<typena
 me boost::add_const<typename T::value_type>::type> I3Frame::Get(const std:
 :string&, typename boost::enable_if<I3Frame::is_shared_ptr<T>, void>::type
 *) const [with T = TestedDataPtr]'
 private/test/wont_compile.cxx:27:   instantiated from here
 /Users/troy/Icecube/meta-projects/offline-software/work-dc2/offline-mk/too
 l-patches/boost/shared_ptr.hpp:201: error: cannot dynamic_cast 'r->boost::
 shared_ptr<const I3FrameObject>::px' (of type 'const class I3FrameObject*
 const') to type 'struct TestedData*' (conversion casts away constness)
 make: *** [/Users/troy/Icecube/meta-projects/offline-software/work-dc2/Mac
 OSX-libstdc++6-ppc/build/icetray/private/test/wont_compile.o] Error 1

wherein the last line is the most important::

 cannot dynamic_cast, conversion casts away constness. 

.. index:: I3_DEFAULT_NAME
.. _I3_DEFAULT_NAME:

I3_DEFAULT_NAME
^^^^^^^^^^^^^^^

Classes that exist as "singletons" in the frame, like I3Geometry,
I3DetectorStatus, I3EventHeader and I3Calibration, or service base
classes like I3RandomService (but not SomePar
ticularKindOfRandomService) are located in standard slots. The
'canonical' I3Geometry, for instance, will always be at slot
"I3Geometry" and never elsewhere. It is possible to put I3Geometries
or I3EventHeaders at locations other than their default names, but
geometries or eventheaders at these locations are not the geometries
and/or event headers, and will be ignored unless following modules
know where to look for them.

The macro I3_DEFAULT_NAME creates the trait types that allow the
I3Frame to determine the default name for some type, which in turn
enables the zero-argument forms of I3Frame::Get<> and I3Context::Get<>
with the macro I3_DEFAULT_NAME. For example the file I3Geometry.h
contains::

 #include <icetray/I3DefaultName.h>
 #include <icetray/I3FrameObject.h> 
 
 struct I3Geometry : public I3FrameObject
 {
   // irrelevant content suppressed
 };
 
 I3_POINTER_TYPEDEFS(I3Geometry);
 I3_DEFAULT_NAME(I3Geometry);

Which then makes it possible to get the geometry as::

 const I3Geometry& geo = frame.Get<I3Geometry>();

The function call above is automatically converted by the
aforementioned traits classes to::

 const I3Geometry& geo = frame.Get<I3Geometry>("I3Geometry");

I3Contexts use this "default name" trait as well. In short, there will
be some classes for which it is not necessary to specify a
name. Currently the list of these classes is I3Geometry,
I3Calibration, I3DetectorStatus, I3EventHeader.

I3Contexts can and do use I3_DEFAULT_NAME as well. With I3Contexts and
the services that they contain, the base class is the important one:
clients will be accessing your
classParticularImplementationOfARandomNumberService as
I3RandomService. It is these service base classes that benefit from
the default name::

 I3RandomService& random = context_.Get<I3RandomService>();

.. _frame-mixing-details:

Frame Mixing
^^^^^^^^^^^^

I3Frames are intended to be processed in an ordered sequence, with keys 
contained in frames of each type ('stream') being made available in later 
frames of other streams. The process by which keys are shared between frames 
of different streams is known as 'mixing'. This makes it easy for I3Modules to 
process only the types of frames which matter to them, while allowing modules 
working primarily with other streams to still conveniently obtain their output. 
For example, a pulse extraction module may run only on DAQ frames, but a 
reconstruction module running only on Physics frames will be able to directly 
access the extracted pulses, as they are 'mixed' from the DAQ frame into the 
Physics frame. 

While a frame has a stream, so does each key stored in the frame. The 
individual keys' streams simply identify whether that key was directly added to
the frame using `Put()`, or whether it was mixed from a preceding frame on 
another stream, and if so from which stream. Directly added keys - those whose
streams match that of the containing frame - are termed 'native'. 

Within any sequence of frames being processed, the following conditions should 
hold:

1. The contents of TrayInfo ('I') and Physics ('P') frames shall not be mixed 
   into any succeeding frames. (These frame types are 'immiscible'.)
2. Every frame shall contain all of the keys from the most closely preceding 
   frame of every other stream which is not 'immiscible' and for which a 
   'native' key was not already present in the frame. 
3. A frame shall contain no non-'native' keys which were not present in the 
   most closely preceding frame on the stream corresponding to each key. 

These rules are intended to be enforced by I3Module and other frame sequence 
handling abstractions provided by IceTray; they should not need to be 
reimplemented by users.


In Python
^^^^^^^^^

.. class:: icecube.icetray.I3Frame
   :noindex:

   The ``I3Frame`` is the main container class for data in the icetray
   framework.  Data including but not limited to detector geometries,
   physics events, and calibrations are all stored in this flexible
   container.

   The ``I3Frame`` is essentially a smart C++ STL ``map`` of
   ``string`` to boost-shared-pointer-to-base, specifically
   ``boost::shared_ptr<I3FrameObject>``.

   .. method:: I3Frame()
      :noindex:

      Create an I3Frame.  By default the stream will be *None*, or ``N``. 

   .. method:: I3Frame(stream)
      :noindex:

      Create an I3Frame on stream *stream*::
      
        >>> frame = I3Frame(I3Frame.Physics)

      *stream* can also be a character, which is used to construct the *Stream* object::

        >>> frame = I3Frame('P')

   .. method:: Has(name)
      :noindex:
   .. method:: __contains__(name)
      :noindex:

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
      :noindex:
   .. method:: __setitem__(where, what)
      :noindex:      

        Put object *what* at slot *where* in the frame.  This is
        also used by ``__setitem__``, so bracket-syntax works::

          >>> obj = icetray.I3Int(775)
          >>> frame.Put('some_int', obj)
          >>> obj2 = icetray.I3Int(767)
          >>> frame['other_int'] = obj2

        The object will be tagged with the frame's default stream.
        See the next form to specify the stream.

   .. method:: Put(where, what, stream)
      :noindex:

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
      :noindex:
   .. method:: __getitem__(where)
      :noindex:

        Get and return object from *where* in the frame.  If the frame
	was read from a file and this is the first attempt to Get the
	item in question, this may trigger the deserialization of the
	item::

	   >>> obj = frame['foo']
	   >>> obj
	   <icecube.icetray.I3Int object at 0xb7d587d4>
           >>> obj = frame.Get('foo')
	   >>> obj
	   <icecube.icetray.I3Int object at 0xb7d587d4>

	This method will throw :exc:`KeyError` if the key isn't found in the
	frame::

           >>> frame['nope']
           Traceback (most recent call last):
             File "<stdin>", line 1, in <module>
           KeyError: 'nope'

   .. method:: keys()
      :noindex:

        Return a list of the frame's keys::

           >>> frame.keys()
           ['blah', 'foo']

   .. method:: values()
      :noindex:

        Return a list of the frame's values::

           >>> frame.values()
           [<icecube.icetray.I3Int object at 0xb7d58824>, <icecube.icetray.I3Int object at 0xb7d587d4>]

   .. method:: items()
      :noindex:

        Return a list of the frame's key/value pairs as 2-tuples::

           >>> frame.items()
	   [('blah', <icecube.icetray.I3Int object at 0xb7d58824>), 
            ('foo', <icecube.icetray.I3Int object at 0xb7d587d4>)]

   .. method:: size()
      :noindex:
   .. method:: __len__()
      :noindex:
 
        Return the number of entries in the frame::

          >>> frame.size()
          2
          >>> len(frame)
          2

        .. note:: Don't confuse this with the version of 'size' that takes a 
	   	  frame object key name.

   .. method:: size(name)
      :noindex:

       Return the size of the buffer associated with frame object
       *name*.  If the frame has not been read from or written to disk
       this buffer may be empty.  Mostly useful for internal testing purposes::

         >>> frame.size('PoleIcLineFit')
         150

   .. method:: __str__()
      :noindex:

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
      :noindex:

      Return the type of the object named *name*::
 
          >>> frame.type_name('PoleIcLinefit')
          'I3Particle'
        
   .. method:: clear()
      :noindex:

      Delete all entries in the frame.

   .. method:: Delete(name)
      :noindex:
   .. method:: __delitem__(name)
      :noindex:

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
      :noindex:

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


   .. method:: get_stop()
      :noindex:

      Return the frame's stop for the given frame key (helpful when dealing with mixed frames)::

        >>> frame.get_stop('I3Geometry')
        >>> icetray.I3Frame.Geometry

      To set the frame's stop::

        >>> frame.stop = icetray.I3Frame.Geometry

.. index:: I3FrameObject
.. _I3FrameObject:

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
