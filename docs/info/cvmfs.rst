
.. highlight:: bash

.. _cvmfs:

CVMFS
=====

:abbr:`CVMFS (CernVM File System)` is a shared filesystem available 
most anywhere around the world.  It has become the standard for
distributing software libraries in the physics community and beyond.
For more details on CVMFS, see the 
`CERN documentation <https://cernvm.cern.ch/fs>`_.

IceCube software is deployed at ``/cvmfs/icecube.opensciencegrid.org``.

Quickstart
----------

.. note::

  If you're installing locally check the :wiki:`wiki <CVMFS>`
  for information on how to configure and mount cvmfs.

From your shell (or in your login script) run::

    eval $(/cvmfs/icecube.opensciencegrid.org/py3-v4.3.0/setup.sh)

This will detect the current OS and architecture and set your 
environment variables appropriately.

* If you are using bash, you can put the above line into your 
  ``.bash_profile`` script to have the environment set on login.

* If you are using zsh, you can put the above line into your 
  ``.zlogin`` script to have the environment set on login.

* If you are running tcsh, you should put the following line into
  your your ``.login`` script. (Note the use of backticks in the 
  expression)::

    eval `/cvmfs/icecube.opensciencegrid.org/py3-v4.3.0/setup.sh`

This should be near the end of any login script, so you don't
accidentally put something else in front of CVMFS on the path.

.. warning::
   
   You should *NOT* put this in your ``.bashrc`` or ``.cshrc`` as this 
   causes the script to modify your environment when subshells are
   started, and can cause Icetray to fail to load.

   If you plan to use the GNU screen utility, you will also want to
   add the following to your ``~/.screenrc``::

      shell -bash

   (or other shell if you don't use bash). The hyphen causes the 
   shell to be started as a login shell, which will keep the software
   environment variables functioning inside your screen session. 

Distributions and Platforms
---------------------------

py3-v4.3
^^^^^^^^

* python 3.11, various python software upgrades over v4.2

* gcc 13.1 (c++23) compiler included

* Supported OS:

  * RedHat / CentOS / Alma:  7 - 9

  * Ubuntu: 20.04, 22.04

  * Debian: 9 - 11

  .. note::

     Works for IceTray releases after June 2023.

py3-v4.2
^^^^^^^^

.. warning::

   py3-v4.2* is deprecated and not supported.

.. note::

   py3-v4.2.1 includes geant4.10.5

* python 3.10, various python software upgrades over v4.1

* gcc 9.3 (c++17) compiler included

* Supported OS:

  * RedHat / CentOS / SL:  7 - 8

  * Ubuntu: 18.04, 20.04

  * Debian: 8 - 10

  .. note::

     Works for IceTray releases after January 2022.

py3-v4.1
^^^^^^^^

.. warning::

   py3-v4.1* is deprecated and not supported.

.. note::

   py3-v4.1.1 includes proper cdk-ncurses dependency.

* python 3.7 and software releases as of late 2019

* gcc 9.2 (c++17) compiler included

* Supported OS:

  * RedHat / CentOS / SL:  7 - 8
  
  * Ubuntu: 16.04 - 18.04
  
  * Debian: 8 - 10
  
  .. note::
  
     Works for IceTray releases after January 2020. 

py3-v4
^^^^^^

.. warning::

   py3-v4.0* is deprecated and not supported.

.. note::

   py3-v4.0.1 includes json-c.
   py3-v4.0.0 is deprecated.

* python 3.6 and software releases as of late 2018

* gcc 7.3 (c++14) compiler included

* Supported OS:

  * RedHat / CentOS / SL:  6 - 7
  
  * Ubuntu: 15.10 - 18.04
  
  * Debian: 8 - 9
  
  .. note::
  
     Works for IceTray releases after April 2019.
     
py2-v3.1
^^^^^^^^

.. note::

   py2-v3.1.1 is a minor fix for Geant4.
   py2-v3.1.0 is deprecated.

* python 2.7 and software releases as of 2017

* GENIE 2.12.8 support

* Supported OS:

  * RedHat / CentOS / SL:  6 - 7
  
  * Ubuntu: 14.04 - 18.04
  
  * Debian: 8 - 9
  
  .. note::
  
     Works for IceTray releases after August 2017.
     Pre-compiled metaprojects available after April 2019. 
     C++11 compiler required.
     
py2-v3
^^^^^^

.. warning::

   py2-v3.0* is deprecated and not supported.

.. note::

   py2-v3.0.1 is a minor fix for some software.
   py2-v3 is deprecated.

* python 2.7 and software releases as of 2017

* Supported OS:

  * RedHat / CentOS / SL:  6 - 7
  
  * Ubuntu: 14.04 - 18.04
  
  * Debian: 8 - 9
  
  .. note::
  
     Works for IceTray releases between August 2017 and April 2019. 
     C++11 compiler required.
     
py2-v2
^^^^^^

.. warning::

   py2-v2 is deprecated and not supported.

* python 2.7 and software releases as of 2015

* Supported OS:

  * RedHat / CentOS / SL:  6 - 7
  
  * Ubuntu: 12.04 - 16.04
  
  * Debian: 8
  
  .. note::

     Works for IceTray releases between April 2016 and August 2017.
     No support after that date.

py2-v1
^^^^^^

.. warning::

   py2-v1 is deprecated and not supported.

* python 2.7 and I3_PORTS

* Supported OS:

  * RedHat / CentOS / SL 6
  
  * Ubuntu 12.04, 14.04
  
  .. note::
  
     Works for IceTray releases before April 2016. No support
     after that date.

     
Self-contained IceTray Scripts
------------------------------

.. highlight:: none

The CVMFS repository includes a wrapper script that you can use as the 
interpreter line in a Python script to automatically load an IceTray 
metaproject environment before execution. This makes the script easier 
to submit to a batch system while also documenting which metaproject it 
is intended to work with. To use it, put a line like the following at 
the top of your Python script::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start
    #METAPROJECT XXXXX

The metaproject specification XXXXX can either be

* a build directory (or a path to an upacked tarball)::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start
    #METAPROJECT /data/user/you/metaprojects/icerec/build

.. note::
   
   Build directories are specific to the OS and architecture where they were
   compiled. If you need to run on multiple OSes, make a tarball for each.

* a tarball URL::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start
    #METAPROJECT http://username:password@convey.icecube.wisc.edu/data/user/your/tarballs/icerec-trunk

.. note::
   
   * `icetray-start` uses a naming convention to find the correct tarball for
     the current OS. If your base name is in your `METAPROJECT` line is
     "icerec-trunk", configure your metaproject with
     `cmake -DCMAKE_INSTALL_PREFIX=icerec-trunk.${OS_ARCH}`. `make tarball` will
     then create an archive with the correct name, e.g.
     "icerec-trunk.RHEL_7_x86_64.tar.gz".
   
   * replace `username:password` with actual credentials (default IceCube ones will work)
     to allow the download of the tarball.

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
^^^^^^^^^^^^^^^^^^^^^^

HTCondor
''''''''

.. highlight:: bash

Specify the path to the Python script as the executable in your submit file::

    Executable = /path/to/foo.py
    Arguments = foo.i3 --verbose bar.i3

Note that when running a HTCondor job, it is advised to use absolute paths, 
e.g. :code:`/path/to/foo.py` instead of :code:`foo.py`. 
This is because the :code:`icetray-start` script will recognize it is in a 
condor job, and does a :code:`cd` to the scratch directory. 
At that point, :code:`foo.py` will no longer point to the right place.


PBS derivatives
'''''''''''''''

.. highlight:: none

Some variants of PBS ignore the shebang line at the beginning of the script.
To force PBS to select the correct interpreter, add a line like the following
anywhere in your script::

    #$ -S /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start

replacing $ with whichever character your PBS flavor uses to denote qsub
options.


.. _pre-compiled-metaprojects:

Pre-compiled metaprojects
-------------------------

The CVMFS repository includes pre-built copies of commonly used versions of
the offline-software, icerec, and simulation metaprojects. To use a 
pre-built metaproject, put a line like the following at the top of your
Python script::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start
    #METAPROJECT: metaproject/VXX-YY-ZZ

for example::

    #!/bin/sh /cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/icetray-start
    #METAPROJECT: combo/stable

