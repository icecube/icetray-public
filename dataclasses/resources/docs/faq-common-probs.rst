FAQ / common errors
-------------------

I can't make pybindings to an I3Vector of my class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

the ``vector_indexing_suite`` requires that the classes inside the
vector have an ``operator==``, so that it is possible to write
constructs like::

  v = I3VectorMyType()
  mt = MyType()

  if mt in v:
     # do something

So if you get deep compiler errors that involve 'operator==', check 
that your class is equality-comparable.


No to_python (by-value) converter found for C++ type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you forget to import project icetray first::

  >>> from icecube import dataio
  >>> f = dataio.I3File("event693.i3")
  >>> frame = f.pop_physics()
  Traceback (most recent call last):
    File "<stdin>", line 1, in <module>
  TypeError: No to_python (by-value) converter found for C++ type: boost::shared_ptr<I3Frame>
  >>> 

then you can't just do it later, due to the way python imports work::

  >>> imp     
  >>> f = dataio.I3File("event693.i3")  # reopen file in same session
  >>> frame = f.pop_physics()
  >>> print frame
  [ I3Frame  (Physics):
    'CalibratedATWD' [Physics] ==> I3Map<OMKey, std::vector<I3Waveform, std::allocator<I3Waveform> > >(14043)
    'CalibratedFADC' [Physics] ==> I3Map<OMKey, std::vector<I3Waveform, std::allocator<I3Waveform> > >(5931)
    'DrivingTime' [Physics] ==> I3Time(38)
    'I3EventHeader' [Physics] ==> I3EventHeader(83)
    'I3EventIdFilter' [Physics] ==> I3Bool(27)
    'I3PfFilterMask' [Physics] ==> I3PfFilterMask(38)
    'I3TriggerHierarchy' [Physics] ==> I3Tree<I3Trigger>(126)
    'IceTopRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >(46)
    'InIceRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >(14282)
  ]

  >>> f = frame['DrivingTime'] 
  /home/troy/Icecube/meta-projects/analysis/trunk/src/icetray/public/icetray/I3Frame.h:293: TRACE: Get<boost::shared_ptr<I3FrameObject const>>("DrivingTime")
  >>> f
  <icecube.icetray.I3FrameObject object at 0x84f12cc>

Where ``f`` above should be an ``icecube.dataclasses.I3Time`` object, not an ``icecube.icetray.I3FrameObject``.

**Remedy**:  start from a fresh python session, and import like this::

  from icecube import icetray, dataclasses, dataio


RuntimeError: extension class wrapper for base class I3FrameObject has not been created yet
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This happens if you import a project that has a wrapped clas that
inherits from :class:`I3FrameObject`, but haven't imported
:mod:`icecube.icetray`, which contains the code for the
:class:`I3FrameObject` wrapper::

  >>> from icecube import dataclasses
  Traceback (most recent call last):
    File "<stdin>", line 1, in <module>
  RuntimeError: extension class wrapper for base class I3FrameObject has not been created yet

You can just import icetray now, but that won't automagically fix the
dataclasses situation, as module :mod:`icecube.dataclasses` has been
marked as imported even though the import failed::

  >>> from icecube import icetray
  >>> foo = dataclasses.I3Time()
  Traceback (most recent call last):
    File "<stdin>", line 1, in <module>
  NameError: name 'dataclasses' is not defined

that's no good.  If you reimport dataclasses, still no help::

  >>> from icecube import dataclasses
  >>> t = dataclasses.I3Time()
  Traceback (most recent call last):
    File "<stdin>", line 1, in <module>
  AttributeError: 'module' object has no attribute 'I3Time'

so you have to :func:`reload` dataclasses::

  >>> reload(dataclasses)
  <module 'icecube.dataclasses' from '/home/troy/Icecube/meta-projects/analysis/trunk/build/lib/icecube/dataclasses.so'>
  >>> t = dataclasses.I3Time()
  >>> t
  <icecube.dataclasses.I3Time object at 0xb36d8dec>

See the python documentation for more information on how ``import``
and ``reload()`` work.  Moral of the story: be careful with the order
of your imports.

