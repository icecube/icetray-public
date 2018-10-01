.. index:: java; JAVA_HOME, jni.h
   pair:   tool; java

.. highlight:: none

.. _JAVA_HOME:
.. _java:

Java
====

.. envvar:: JAVA_HOME

   Java is detected via the ``JAVA_HOME`` environment variable.  *If*
   your workspace contains projects that use java, this variable must
   be set and exported at cmake time.

A successful detection should create output that looks something like
this (at cmake time)::

  -- jni
  -- + jni.h found at /usr/lib/jvm/java-1.5.0-sun/include
  -- + jni_md.h found at /usr/lib/jvm/java-1.5.0-sun/include/linux
  -- + jvm library found at /usr/lib/jvm/java-1.5.0-sun/jre/lib/i386/server/libjvm.so
  -- + verify library found at /usr/lib/jvm/java-1.5.0-sun/jre/lib/i386/libverify.so
  -- + zip library found at /usr/lib/jvm/java-1.5.0-sun/jre/lib/i386/libzip.so
  -- + JAVA_HOME=/usr/lib/jvm/java-1.5.0-sun

An incorrectly or unset environment variable gets you this kind of
output::

  -- jni
  -- - jni.h
  -- - jni_md.h
  -- - jvm library
  -- - verify library
  -- - zip library
  -- - JAVA_HOME (/path/to/nothing) is either unset or does not point to a valid directory.
  -- - This is only an error if some project in your workspace requires java.

And if you indeed have projects that need java, you will later get a
proper error (not just a warning) that includes text such as::

  CMake Error: The following variables are used in this project, but they 
  are set to NOTFOUND.
  Please set them or make sure they are set and tested correctly in the 
  CMake files:
  jni_h_include_dir
     used as include directory in directory 
  /src/V02-02-09/src/c2j-icetray
     used as include directory in directory 
  /src/V02-02-09/src/mmc-icetray
  
.. _jni.h:

The java tool looks for the files ``jni.h`` and others (as listed in
the 'successful detection' block above.)  See the source code in
``$I3_SRC/cmake/tools/Findjni.cmake`` for full details.

.. warning::

   If you think you have fixed your environment variables but errors
   still persist, try removing ``CMakeCache.txt`` and rerunning
   ``cmake``.

