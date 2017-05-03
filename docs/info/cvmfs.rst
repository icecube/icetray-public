
.. highlight:: bash

.. _cvmfs:

CVMFS
=====

CVMFS (CernVM File System) is a shared filesystem available 
most anywhere around the world.  It has become the standard for
distributing software libraries in the physics community and beyond.
For more details on CVMFS, see the 
`CERN documentation <https://cernvm.cern.ch/portal/filesystem>`_.

IceCube software is deployed at `/cvmfs/icecube.opensciencegrid.org`.

Quickstart
----------

.. note ::

  If you're installing locally check the `wiki <https://wiki.icecube.wisc.edu/index.php/CVMFS>`_ 
  for information on how to configure and mount cvmfs.

From your shell (or in your login script) run::

    eval $(/cvmfs/icecube.opensciencegrid.org/py2-v1/setup.sh)

This will detect the current OS and architecture and set your 
environment variables appropriately.

* If you are using bash, you can put the above line into your 
  `.bash_profile` script to have the environment set on login.

* If you are using zsh, you can put the above line into your 
  `.zlogin` script to have the environment set on login.

* If you are running tcsh, you should put the following line into
  your your `.login` script. (Note the use of backticks in the 
  expression)::

    eval `/cvmfs/icecube.opensciencegrid.org/py2-v1/setup.sh`

This should be near the end of any login script, so you don't
accidentally put something else in front of CVMFS on the path.

.. warning::
   
   You should *NOT* put this in your `.bashrc` or `.cshrc` as this 
   causes the script to modify your environment when subshells are
   started, and can cause Icetray to fail to load.

   If you plan to use the GNU screen utility, you will also want to
   add the following to your .screenrc::

      shell -bash

   (or other shell if you don't use bash). The hyphen causes the 
   shell to be started as a login shell, which will keep the software
   environment variables functioning inside your screen session. 

Distributions and Platforms
---------------------------

* py2-v1 - python 2.7 and I3_PORTS

  * RedHat / CentOS / SL 6
  
  * Ubuntu 12.04, 14.04
  
  .. note:
  
     Works for IceTray releases before April 2016. No support
     after that date.

* py2-v2 - python 2.7 and software releases as of 2015

  * RedHat / CentOS / SL:  6 - 7
  
  * Ubuntu: 12.04 - 16.04
  
  * Debian: 8
  
  .. note:
  
     Works for IceTray releases after April 2016.

Self-contained IceTray Scripts
------------------------------

.. highlight:: none

The CVMFS repository includes a wrapper script that you can use as the 
interpreter line in a Python script to automatically load an IceTray 
metaproject environment before execution. This makes the script easier 
to submit to a batch system while also documenting which metaproject it 
is intended to work with. To use it, put a line like the following at 
the top of your Python script::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start
    #METAPROJECT XXXXX

The metaproject specification XXXXX can either be

* a build directory::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start
    #METAPROJECT /data/user/you/metaprojects/icerec/build

* a tarball URL::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start
    #METAPROJECT http://convey.icecube.wisc.edu/data/user/you/tarballs/icerec-trunk

* one of the :ref:`pre-compiled-metaprojects` distributed through the 
  CVMFS repository

Then, make your script executable and invoke it directly::

    ./foo.py

The icetray-start script performs the following tasks:

* Sets up the toolset for the current OS and architecture

* Detects a suitable HTTP proxy and sets absolute paths to the user's 
  GridFTP proxy certificate, if present

* Locates and downloads the metaproject tarball if necessary

* Executes the Python script inside the metaproject's `env-shell.sh`

Use with batch systems
""""""""""""""""""""""

HTCondor
''''''''

.. highlight:: bash

Specify the path to the Python script as the executable in your submit file::

    Executable = foo.py
    Arguments = foo.i3 --verbose bar.i3

PBS derivatives
'''''''''''''''

.. highlight:: none

Some variants of PBS ignore the shebang line at the beginning of the script.
To force PBS to select the correct interpreter, add a line like the following
anywhere in your script::

    #$ -S /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start

replacing $ with whichever character your PBS flavor uses to denote qsub
options.

.. _pre-compiled-metaprojects:

Pre-compiled metaprojects
-------------------------

The CVMFS repository includes pre-built copies of commonly used versions of
the offline-software, icerec, and simulation metaprojects. To use a 
pre-built metaproject, put a line like the following at the top of your
Python script::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start
    #METAPROJECT: metaproject/VXX-YY-ZZ

for example::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py2-v1/icetray-start
    #METAPROJECT: icerec/V04-06-00

