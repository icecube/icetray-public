Ubuntu or Debian Variants
^^^^^^^^^^^^^^^^^^^^^^^^^

**Its is highly recommended to use Long-Term Support (LTS) releases of Ubuntu.**

Using the Package Manager
"""""""""""""""""""""""""

On laptops or user-controlled workstations it is recommended to install
as much as possible through the package manager.

Basic Packages (offline-software)
.................................

For a basic system (such as viewing files, basic tasks), install:

.. container:: wrapped-code

    apt-get install build-essential cmake libbz2-dev libgl1-mesa-dev 
    freeglut3-dev libxml2-dev subversion libboost-python-dev 
    libboost-system-dev libboost-signals-dev libboost-thread-dev 
    libboost-date-time-dev libboost-serialization-dev libboost-filesystem-dev 
    libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev 
    libgsl0-dev libcdk5-dev libarchive-dev python-scipy ipython-qtconsole 
    libqt4-dev python-urwid

For newer Ubuntu versions (>=14.04):

.. container:: wrapped-code

    apt-get install libz-dev libqt5opengl5-dev libstarlink-pal-dev
    python-sphinx libopenblas-dev

Simulation Packages
...................

Simulation doesn't require too much extra. cfitsio is required, the rest
is optional depending on your use case:

.. container:: wrapped-code

    apt-get install libcfitsio3-dev libsprng2-dev libmysqlclient-dev
    libsuitesparse-dev

IceRec Packages
...............

Some packages are easily available:

.. container:: wrapped-code

    apt-get install libcfitsio3-dev libmysqlclient-dev libhdf5-serial-dev

Root is only available in newer releases (Ubuntu 13.10+, Debian 7+):

.. container:: wrapped-code

    apt-get install root-system

Other Tools
...........

If you want Geant4 or Genie, you'll need to use I3_PORTS.

Using I3_PORTS
""""""""""""""

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/Ubuntu.sh
that will install the necessary packages.

To install further packages (emacs and so forth), the easiest thing to
do is use the "Synaptic Package Manager", which you can start from menu
``System --> Administration``. 

