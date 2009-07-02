Tool Build System - Setting up your I3_PORTS toolset
=====================================================

Tools are external packages that are required dependencies for IceCube
Offline software packages. These are distributed as source and built
on your local cluster using the Ports system. This will help you setup
your toolset (often referred to as I3_PORTS.

There is a single "meta-package" that collects all needed tools into a
single, one-stop-shopping experience.  The current toolset is::

   i3-tools-v3


Quick start for the impatient
------------------------------
The MacPorts two-step:

* Perform a checkout of the project's latest copy of the MacPorts
  source from Subversion::

    svn co http://code.icecube.wisc.edu/svn/tools/DarwinPorts/trunk ~/port_source

* Build MacPorts and the toolset

 * Change your working directory to ~/port_source.  This can be any
   directory where you can checkout and compile the MacPorts tool
   system::

     cd ~/port_source

 * Define $I3_PORTS location.  This will be the location where your
   new toolset will be installed.  Make sure you have permissions to
   write in this area.  It is recommended that you add this to your
   startup files (bash_profile/cshrc)::

     export I3_PORTS=/path/to/toolspace  (setenv I3_PORTS /path/to/toolspace)

 * Execute i3-install.sh passing $I3_PORTS as the sole argument::

    ./i3-install.sh $I3_PORTS

 * i3-install.sh will configure, compile, and install the MacPorts
   distribution.  Afterwards it will begin compiling the entire
   Offline Software toolset within MacPorts.  Go read the day's news
   or make a lasagna while you're at it - this step will take some
   time and is significantly quicker on a fast system.

Important Notes
^^^^^^^^^^^^^^^^

* **NEVER** manage the port installation as user 'root'.  Running port
  as user root will allow you to install over existing system
  software, and possibily cause harm to your system if your $I3_PORT
  variable somehow gets set incorrectly.  You should always use a
  non-priveledged account to install the IceCube tools system.

* Platform-specific notes and minor required changes can be found at
  the bottom of this page.  Please read the notes appropriate to your
  platform before beginning installation.

Dependencies
^^^^^^^^^^^^^^

The new tools system depends on:

* Subversion
* Tcl, /usr/lib/tclConfig.sh, and Tcl header files
* OpenSSL header files
* Curl
* X11 libraries (required by ROOT) - specifically, /usr/X11R6/lib/libX11.so must be present (it is usually a symbolic link pointing to /usr/X11R6/lib/libX11.so.?)
* bzip2 header file (/usr/include/bzlib.h) and libraries (required by Boost build)

(To get those header files on RedHat systems, install the 'tcl-devel'
and/or 'openssl-devel' packages.)

Getting started - Manually compiling tools for your personal development

Most people should just use the Quickstart guide
[[New_Tool_Build_system#Quick_start_for_the_impatient]]

The individual steps
---------------------

For an even more manual installation, the following steps are those
i3-install.sh will perform for you.

* Perform a checkout of the project's latest copy of the MacPorts source from Subversion::

   svn co http://code.icecube.wisc.edu/svn/tools/DarwinPorts/trunk <path_to_port_src>

* Change your working directory to <path_to_port_src>.  Configure the MacPorts source tree with the installation location::

    cd <path_to_port_src> && ./configure --prefix=$I3_PORTS

* Compile and install MacPorts::

    make && make install

* Change your working directory to $I3_PORTS.  Fetch a set of portfiles::

    cd $I3_PORTS && ./bin/port -dv sync

* Install the complete IceCube Offline Software tools distribution::

    ./bin/port -dv install i3-tools-v2

Getting the test-data
-----------------------

For the unit test and test scripts, a selection of small test-data
files has been prepared.  These files traditionally were rsynced along
with the binary tools.  Now this process has been split from the
I3_PORTS tool installation process.

The tools are stored in a directory "test-data" which will live in
your I3_PORTS directory.

* If you have write permission in your I3_PORTS directory, from any
  I3_PORTS-based workspace run::

    make rsync 

* If your I3_PORTS is not generally writeable by you (a group or
  software install account), then run the following command with
  appropriate write permissions::

   rsync -vrlpt code.icecube.wisc.edu::Offline/test-data $I3_PORTS/

Updates to tools
-------------------

When new tools or changes to existing tools happen, you will need to
update your tool installation.

How will I know new tools are ready?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A few points regarding updates to tools.

* New additions to the PORT tools will truly be additions, with a new
  version living side-by-side with the older version of the tools.
  This will allow older releases to continue working.

* Which tools is actually used is determined by settings in the cmake
  project.

* Announcements of new tool requirements will be included in
  offline-software release announcements (sent to icecube-c).

* Development announcements (such as the availability of new tools for
  development use) will be announced only on the dataclass mailing
  list.

How do I upgrade an existing I3_PORTS installation?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The port system allows for easy upgrading of your tool installation.  Here are some simple steps::

  cd $I3_PORTS 

Update your tool list from the server::

  ./bin/port -vd sync 

Now update your tool installs (upgrade the tool meta-package)::

  ./bin/port -vd upgrade i3-tools-v2  

If you know that there is a specific package you want to force a rebuilt of, you can::

  ./bin/port -fvd install <tool name> 

Compiling tools for one or several platforms for a cluster installation
------------------------------------------------------------------------

Installing the Icecube tools for a cluster installation is almost the
same as for a single installation.  Please keep the following things
in mind:

* Do not install the toolsets as user root (super-user).  The IceCube
  port system should be kept separate from your system installation.

  * We recommend you manage the tool installations using a dedicated
    account (user=icecube-sw or similar)

Essentially, you will be making several IceCube tool installations into a common,
shared disk, one for each OS/Architecture you are working with:

* Make a common area on your shared NFS disk for your tool installs to live in::
  
    mkdir /path/to/NFS/disk (example: /disk02/software/i3tools)</pre>

* Make sure this area is writable by your dedicate account user.

* Make sure you have a dedicated machine that meets all the prerequisites for each OS/Archicture that you wish to build tools for.
* On each OS/Architecture::

     svn co http://code.icecube.wisc.edu/svn/tools/DarwinPorts/trunk port_src

  * port_src directory can be, but doesn't have to be on the shared NFS disk.
  * cd port_src
  * ./i3-install.sh /path/to/NFS/disk/<arch-ident> (example: ./i3-install.sh /disk02/software/i3tools/rhel4-i386)
  * <arch-ident> will be the ports installation for that OS/Arch.
  * <arch-ident> can NOT contain "+" (so don't use: Linux-libstdc++6-i386 or similar)
  * Make a single rsync of the test-data.
     * rsync -vrlpt code.icecube.wisc.edu::Offline/test-data /path/to/NFS/disk/
     * This will create a single copy of the test-data
  * Link this toolset to each installed toolset::

      ln -s /path/to/NFS/disk/test-data /path/to/NFS/disk/<arch-ident>

Troubleshooting
----------------------------------------------------------

* If your :file:``i3-install.sh`` build gets interrupted for some
  reason (other than an error), there is no need to remove all your
  previous work and start from scratch.  You can take a look at the
  contents of the i3-install.sh file.  As long as you are past the
  "make install" step (first couple of minutes), you can simply repeat
  any of the subsequent commands ($1 in this script is your :env:`$I3_PORTS`
  directory) This is essentially switching to the manual instructions.


See also :ref:`platforms`.
