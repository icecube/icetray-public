.. _nlopt-tool:

=====
NLopt
=====

`NLopt <http://ab-initio.mit.edu/wiki/index.php/NLopt>`_ is a free/open-source library for nonlinear optimization, providing a common interface for a number of different free optimization routines available online as well as original implementations of various other algorithms. It is used as an optional dependency in the :ref:`lilliput` project by the :js:data:`I3GulliverNLoptFactory` service as a minimization plugin for the :ref:`gulliver` suite.

Linux distribution such as debian do include an NLopt package, unfortunately they do not include the c++ interface. So it may be necessary to install from source.

Apple Mac OS
------------

On Mac OS, homebrew can be used:

.. code-block:: bash
 
  brew tap homebrew/science
  brew install homebrew/science/nlopt

If you are interested in using NLopt's python interface independently of IceTray you can add the ``--with-python`` flag.


Install from Source
--------------------

IceCube uses the c++ interface to NLopt and requires shared libraries, both of which must be turned on by command line options during configure. 

.. code-block:: bash

  wget http://ab-initio.mit.edu/nlopt/nlopt-2.4.2.tar.gz
  tar xzvf nlopt-2.4.2.tar.gz
  cd nlopt-2.4.2.tar.gz
  ./configure --with-cxx --enable-shared
  make
  sudo make install

See Also
--------

- `cvmfs script <https://github.com/WIPACrepo/cvmfs/blob/master/builders/tools/nlopt.py>`_
- `homebrew script <https://github.com/Homebrew/homebrew-science/blob/master/nlopt.rb>`_
