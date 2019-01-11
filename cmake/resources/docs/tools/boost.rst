.. index:: boost
   pair:   tool; boost
   single: boost; setting version
   single: boost; and python
   single: python; and boost

.. highlight:: sh

Boost
=====

Boost is available in the toolset in versions 1.33.1 and 1.36.0.

The most recent (highest version number) will be used by default.  If
you e.g. have both installed and want to force the use of boost
1.33.1, set ``BOOST_VERSION`` on the cmake line::

  % cd build
  % cmake -DBOOST_VERSION=1.33.1 ../src

.. warning::

   You have to *delete* your ``CMakeCache.txt`` if you want to switch
   versions in a configured workspace.  The include/library paths are
   not reset if ``BOOST_VERSION`` is changed. 

boost python bindings library
-----------------------------

There is a history of missteps regarding this library and the toolset.
We appear to have converged on a good solution.

In chronological order:

1.  Python (2.3.5) was in the toolset, and the boost_python library was
    built by the boost port against this python.  I don't recall at this
    point why we put python in the toolset.

2.  We discovered that having python in the toolset was a huge hassle:
    it was hard to install other python packages into this toolset
    python, and it was very easy to use the wrong interpreter.  This
    got you errors about mismatching python ABIs and/or runtime link
    errors.  You had to distribute the entire python out to cluster
    nodes when doing mass processing.  

    So we took python out of the toolset, or rather, the cmake build
    ignored the python in the toolset.  This also means our
    boost_python library had to be built by the metaproject itself
    (you can't use boost_python built against a toolset python 2.3.5
    if the metaproject is building against a system python 2.4.2)
    
    We created a project 'boost' which contained code to boost_python
    (it got a few more libraries that needed patching as well), and
    boost_python was built along with the rest of the metaproject,
    against the system python.

    At this point cmake also created a link::
    
      $I3_BUILD/bin/python -> /usr/bin/python

    So that if you still had a toolset with python 2.3.5 in it, your
    shell wouldn't catch that one and screw things up.

    This is the final state for boost-1.33.1.  You could move the
    build of boost_python back into the toolset but we don't have
    anybody to do the necessary hand-holding.
    
3.  As of boost-1.36.0, boost_python is built by the toolset against
    the system python.  

