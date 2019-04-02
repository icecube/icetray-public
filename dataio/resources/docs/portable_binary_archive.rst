The IceCube Portable Binary Archive Format
==========================================

I3Files are actually portable binary archives, modelled after an old
Boost archive format. While it was removed from Boost, the archive
lives on in IceCube software.  This document attempts to explain the
format for anyone doing very, very low-level work with I3Files.

.. caution::

    If you don't really need to know this, run now. Really, we mean it.

History
-------

2006:

    Decide we like portable binaries better than xml files or Root files.

    Troy D. Straszheim creates this portable binary format using the Boost
    (1.34) third-party portable binary archive.

2009:

    Move to Boost 1.38. The STL container serialization changed,
    so use patches in cmake to bring back the old format for us.

2011:

    Claudio Kopper adds support for reading the binary archive in python,
    mostly to support pickling of I3Frames and I3FrameObjects.

2012:

    Nathan Whitehorn decouples IceTray from Boost 1.38 by copying the
    portable binary archive to IceTray.

2013:

    Updates to be compatible with the newest version of Boost
    (1.52 at the time).

2015:

    Boost 1.58 makes large changes to the internals, rendering
    our patches invalid. A new strategy is needed. For now, ban all
    boost versions >= 1.58.

2016:

    The new strategy is to fork boost::serialization at 1.57 and
    embed this into all software.  Not ideal, but a good patch.

The *Rules*
-----------

First, some information on the size of various header objects:

    ===================  ========  ============
    Name                 Type      Size (bytes)
    ===================  ========  ============
    tracking             uint8_t   1
    version              uint8_t   1
    class_id             int16_t   2
    class_id_reference   int16_t   2
    class_id_optional    NONE      0
    class_name           NONE      0
    object_id            uint32_t  4
    object_reference     uint32_t  4
    collection_size      uint32_t  4
    ===================  ========  ============

Note that everything is saved as little endian. A converter is used on
big endian platforms to achieve this portability. For more info on
endianness, read `the wikipedia article <http://en.wikipedia.org/wiki/Endianness>`_.

And now some pseudo-code for serialization (writing to binary format)
and deserialization (reading from binary format).

Serialization
^^^^^^^^^^^^^

of objects
++++++++++

::

    if serialized through pointer:
        save_data
    else:
        if class_info and !initialized:
            class_id_optional
            tracking
            version
        if !tracking:
            save_data
        elif new object:
            object_id
            save_data
        else:
            object_reference(object_id)

of pointers
+++++++++++

::

    if !initialized:
        class_id
        if unregistered_class and is_polymorphic:
            class_name
        if class_info:
            tracking
            version
    else:
        class_id_reference(class_id)
    if !tracking:
        save_object_ptr
    elif new_object:
        object_id
        save_object_ptr
    else:
        object_reference(object_id)

Deserialization
^^^^^^^^^^^^^^^

of objects
++++++++++

::

    if serialized through pointer:
        save_data
    else:
        if !initialized:
            if class_info
                class_id_optional
                tracking
                version
        if tracking:
            object_id or object_reference
        if !is_object_reference:
            load_data

of pointers
+++++++++++

::

    class_id
    if is_null_pointer:
        return
    if is_abstract or is_polymorphic:
        class_name
    if !intialized:
        if class_info:
            class_id_optional
            tracking
            version
    if tracking:
        object_id or object_reference
    if !is_object_reference:
        load_object_ptr


I3FrameObject
-------------

The basic building blocks in IceCube software are made up of I3FrameObjects.
Thus, this is where serialization begins.

I3FrameObject is an abstract class used to hold things that will go into an
I3Frame. This requires that the object be serializable in order to be
saved to file. See :doc:`serialization` for more details about how to create
such objects, but just know that I3FrameObject is the base class.

The basic structure of the serialized blob is like such:

* (your class) - 4 bytes

  * class_id - 2 bytes

  * tracking_id - 1 byte

  * version_id - 1 byte

* I3FrameObject base class - 8 bytes

  * class_id - 2 bytes

  * tracking_id - 1 byte

  * version_id - 1 byte

  * object_id - 4 bytes

* (serialized data for your class)

A very common binary blob header is 0x010000000000010001000000.
This is because the I3FrameObject always has tracking enabled, version 0,
and is the 0th class and 1st object serialized.

.. tip::

    The easiest way to obtain the binary blob in python is::

        frame['MyObject'].__getstate__()[1]

    or, if you just have the object directly::

        my_object.__getstate__()[1]

    Note that this only works on objects that use the dataclass_suite for pybindings.

Internal Objects
^^^^^^^^^^^^^^^^

If your I3FrameObject is not just a primitive (bool, int, double, string),
but is instead composed of other objects, those need to be serializable as
well.

There are two main cases, tracking vs non-tracking.  In the tracking case,
objects will have a header that will vary depending on if this is the first
time the type is seen.

Tracking - First Occurance
++++++++++++++++++++++++++

* Object Header - 8 bytes

  * class_id - 2 bytes

  * tracking_id - 1 byte

  * version_id - 1 byte

  * object_id - 4 bytes

* (serialized data)

Tracking - Second (or later) Occurance
++++++++++++++++++++++++++++++++++++++

* Object Header - 4 bytes

  * object_id - 4 bytes

* (serialized data)


In the no tracking case, the class just needs to be registered the first
time, and all other occurances are strictly the serialized data of the
object.

No Tracking - First Occurance
+++++++++++++++++++++++++++++

* Object Header - 2 bytes

  * class_id - 2 bytes

* (serialized data)

No Tracking - Second (or later) Occurance
+++++++++++++++++++++++++++++++++++++++++

* (serialized data)

Standard Library Containers
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A vector needs some special consideration because of optimizations
based on contents. Lists and maps will follow the un-optimized approach.

Vector Optimization
+++++++++++++++++++

If the vector contents are a primitive datatype, the array inside the
vector can be serialized directly from memory:

* Count - 4 bytes

* memory copy of array - (sizeof(type) * count)

Non-Optimized Container
+++++++++++++++++++++++

* Count - 4 bytes

* for each object in the container:

  * (serialized object)

Maps serialize each object as a std::pair of key,value.

I3Frame
-------

An I3Frame is basically a map of frame object names to serialized
I3FrameObjects.

A detailed format is:

* I3 tag '[i3]' - 4 bytes (0x5b69335d)

* version - 4 bytes

* stream - 3 bytes

  * tracking_id - 1 byte

  * version_id - 1 byte

  * value - 1 byte

* size - 4 bytes

* for each frame object:

  * key - [string]

  * type_name - [string]

  * buf - [serialized I3FrameObject]

* checksum - 4 bytes

.. note::

    The checksum is currently a crc32 checksum with the following bytes
    going into it in this order:

    * stream value

    * size

    * for each frame object:

      * key

      * type_name

      * buf


I3File
------

I3Files are now fairly straightforward - just a bunch of I3Frames. Since the
archive format is a stream of binary data, I3Files are just one serialized
I3Frame after another.

This creates a few interesting effects. First, and most negatively, there is
no header for seeking directly to the Nth frame. (this is a much desired
feature if you want to implement it :) On the positive side, I3Files can be
any file-like object, including pipes or network sockets. This makes it easy
to do live streaming or directly writing to disk as processing happens,
without storing the whole file in memory.


Serialization Examples
----------------------

Now that we have some theoretical knowledge, let's go through some examples
of how different objects are serialized.

I3Int
^^^^^

Say we have an I3Int(10).  The serialization is::

    0x0100000000000100010000000a000000

We break this up into pieces:

* I3Int - 4 bytes (0x01000000)

  * class_id - 2 bytes = 1

  * tracking_id - 1 byte = 0

  * version_id - 1 byte = 0

* I3FrameObject base class - 8 bytes (0x0000010001000000)

  * class_id - 2 bytes = 0

  * tracking_id - 1 byte = 1

  * version_id - 1 byte = 0

  * object_id - 4 bytes = 1

* The actual int - 4 bytes (0x0a000000) = 10

I3Double
^^^^^^^^

Say we have an I3Double(3.14159).  The serialization is::

    0x0100000000000100010000006e861bf0f9210940

We break this up into pieces:

* I3Double - 4 bytes (0x01000000)

  * class_id - 2 bytes = 1

  * tracking_id - 1 byte = 0

  * version_id - 1 byte = 0

* I3FrameObject base class - 8 bytes (0x0000010001000000)

  * class_id - 2 bytes = 0

  * tracking_id - 1 byte = 1

  * version_id - 1 byte = 0

  * object_id - 4 bytes = 1

* The actual double - 8 bytes (0x6e861bf0f9210940) = 3.14159

I3String
^^^^^^^^

Say we have an I3String('testing').  The serialization is::

    0x01000000000001000100000000700000074657374696367

We break this up into pieces:

* I3String - 4 bytes (0x01000000)

  * class_id - 2 bytes = 1

  * tracking_id - 1 byte = 0

  * version_id - 1 byte = 0

* I3FrameObject base class - 8 bytes (0x0000010001000000)

  * class_id - 2 bytes = 0

  * tracking_id - 1 byte = 1

  * version_id - 1 byte = 0

  * object_id - 4 bytes = 1

* The string length - 4 bytes (0x07000000) = 7

* The actual string - 7 bytes (0x74657374696367) = 'testing'

OMKey
^^^^^^^

Say we have an OMKey(25,45,0).  The serialization is::

    0x01010000000000190000002d00000000

Note that this is a little different than normal because the OMKey is not
meant to be directly added to the frame. The object is not an I3FrameObject,
and serializes differently to reflect that.

We break this up into pieces:

* OMKey - 6 bytes (0x010100000000)

  * tracking_id - 1 byte = 1

  * version_id - 1 byte = 1

  * object_id - 4 bytes = 0

* string number - 4 bytes (0x19000000) = 25

* OM number - 4 bytes (0x2d000000) = 45

* PMT number - 1 byte (0x00) = 0

I3VectorOMKey
^^^^^^^^^^^^^

Say we have an I3VectorOMKey() with [OMKey(35,56,0),OMKey(25,45,0)].
The serialization is::

    0x01000000000001000000000002000000010102000000230000003800
    0x00000003000000190000002d00000000

We break this up into pieces:

* I3VectorOMKey - 4 bytes (0x01000000)

  * class_id - 2 bytes = 1

  * tracking_id - 1 byte = 0

  * version_id - 1 byte = 0

* I3FrameObject base class - 8 bytes (0x0000010001000000)

  * class_id - 2 bytes = 0

  * tracking_id - 1 byte = 1

  * version_id - 1 byte = 0

  * object_id - 4 bytes = 1

* Vector base class - 2 bytes (0x0000)

  * tracking_id - 1 byte = 0

  * version_id - 1 byte = 0

* Count - 4 bytes (0x02000000) = 2

* OMKey - 6 bytes (0x0101020000)

  * tracking_id = 1

  * version_id = 1

  * object_id = 2

* string number - 4 bytes (0x23000000) = 35

* OM number - 4 bytes (0x38000000) = 56

* PMT number - 1 byte (0x00) = 0

* OMKey - 4 bytes (0x030000)

  * object_id = 3

* string number - 4 bytes (0x19000000) = 25

* OM number - 4 bytes (0x2d000000) = 45

* PMT number - 1 byte (0x00) = 0



[To Do: fill in more examples]

