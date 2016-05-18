Apple Mac OS X
^^^^^^^^^^^^^^

IceTray requires some additional support software to build and function. Before
you gallop off and try to install all of the dependencies by hand, please read
these notes in their entirety. Especially:

.. warning::

   Do not try to install your own Python over the perfectly good version
   shipped with the base system. It is very likely to end in tears. This
   includes the Enthought and Anaconda distributions as well as the Python
   formula in homebrew; they do not play nicely with IceTray. See
   :ref:`osxpythonsetup` for more information.

Developer Tools
"""""""""""""""

You'll need a compiler in order to build the IceCube software. Apple distributes
clang and llvm-gcc both as a stand-alone download and as part of its Xcode IDE,
but requires you to register for an `Apple ID`_  before you can download either
one. The Xcode download is 10 times larger than the command-line tools package,
so you should only download it if you need it for non-IceCube work.

.. _`Apple ID`: https://appleid.apple.com

Command-Line Tools
..................

1) Go to the `Apple Developer Downloads page <http://developer.apple.com/downloads>`_,
   signing in with your `Apple ID`_.
2) Download and install the latest Command Line Tools.

Alternativly you can just type `xcode-select --install` at the command-line to install
the command line tools.

Xcode
.....

1) Download Xcode from the App Store.

2) Open Xcode, and open the Preferences window (Xcode > Preferences).

3) In the :ref:`Xcode4Prefs`, click the Downloads tab, and install the
   Command Line Tools.

.. _Xcode4Prefs:

.. figure:: ../figs/xcode4_prefs.png
	:width: 432px
	
	Xcode Preferences window

After the install finishes you should have both clang and llvm-gcc:

.. code-block:: sh
	
	bash-3.2$ g++ --version
	i686-apple-darwin11-llvm-g++-4.2 (GCC) 4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.11.00)
	Copyright (C) 2007 Free Software Foundation, Inc.
	This is free software; see the source for copying conditions.  There is NO
	warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	bash-3.2$ clang++ --version
	Apple clang version 4.0 (tags/Apple/clang-421.0.60) (based on LLVM 3.1svn)
	Target: x86_64-apple-darwin12.0.0
	Thread model: posix

Package managers
""""""""""""""""

As noted in :ref:`do-i-need-i3ports`, you can set up your environment quite
quickly if you use a package manager. While Apple doesn't provide an official
package manager for OS X, there are a number of 3rd implementations to choose
from.

.. _Homebrew: http://brew.sh
.. _MacPorts: https://www.macports.org
.. _Fink: http://www.finkproject.org

Homebrew
........

Homebrew_ is probably the easiest way to install packages on OS X, and
distributes the most heavy-weight dependencies (cmake, boost, and Qt) as binary
packages. Most of the required formulae are in the main distribution, but you
should also `tap`_ homebrew/science and IceCube-SPNO/icecube. Install them like
this::

	brew install cmake

The following formulae are recommended:

* offline-software: boost boost-python cmake cdk gsl hdf5 libarchive qt pyqt pal doxygen wget
* IceRec: cfitsio minuit2 suite-sparse healpix multinest nlopt
* simulation: sprng2

.. warning:: Newer versions of boost have broken our serialization
   methods. This a known issue that is being worked on. In the
   meantime it is recommended that you use boost v1.55. This can be
   installed from Homebrew with::

       brew tap homebrew/versions
       brew install boost155 --with-python && brew link boost155

.. warning:: Some Homebrew formulas have Python as a dependency, so a
   second Python may sneak onto your computer without your
   knowledge. To avoid this, install formulas that depend on python
   with the parameter ``--build-from-source``. See `Homebrew's notes
   on Python`_ for further information.

.. _tap: https://github.com/Homebrew/homebrew/blob/master/share/doc/homebrew/brew-tap.md
.. _`Homebrew's notes on Python`: https://github.com/Homebrew/homebrew/blob/master/share/doc/homebrew/Homebrew-and-Python.md#why-is-homebrews-python-being-installed-as-a-dependency

The version of boost which is provided by homebrew is incompatable with IceTray, you need to use an older version.
Homebrew provides version 1.55 in its versions tap.::

	brew install homebrew/versions/boost155 --with-python
	brew link -f homebrew/versions/boost155

However, many homebrew packages will still try to install newer versions of boost, which will cause problems. I am not aware of any way to prevent this other than not installing any homebrew package which depend on boost, mysql is one such package.

MacPorts
........

I3_PORTS shares a common ancestor with MacPorts_. As a consequence, they both
use :command:`port` but expect to be managed at different privilege levels,
which can lead to all sorts of subtle inconsistencies and outright breakage if
you confuse them. If you can at all avoid it, do not use I3_PORTS and MacPorts_
together. If you must, however, here are some pitfalls to avoid:

* Always explicitly run :command:`$I3_PORTS/bin/port` (never let it come from
  your path).

* Never :command:`su` or :command:`sudo` from a terminal windows where you
  have run :command:`env-shell.sh`, as this altered path might be kept and
  cause conflicts when you think you are running a "system" :command:`port`
  command. Start a new terminal instead.

Fink
....

Does anyone still use Fink_? If you do, and think it's any good, write some
documentation.

ROOT on OS X
""""""""""""

Building ROOT dictionaries on OS X Mavericks and beyond is no longer
supported by ROOT. By default, ROOT dictionaries are not built.

.. warning:: ROOT has been known to completely break builds on OS X Mavericks. If this happens to you, build with :command:`cmake -DUSE_ROOT=OFF ...`. For more information, see the `original bug report`_.

.. _`original bug report`: http://code.icecube.wisc.edu/projects/icecube/ticket/796

ROOT can be installed with homebrew::

   brew install --build-from-source homebrew/science/root

If you get an error message like this:

.. code-block:: none

   Error: cannot open file "AvailabilityMacros.h" include/RConfig.h:376:
   Warning: Error occurred during reading source files

   Warning: Error occurred during dictionary source generation

   !!!Removing core/base/src/G__Base1.cxx core/base/src/G__Base1.h !!!

   Error: core/utils/src/rootcint_tmp: error loading headers...

   make: *** [core/base/src/G__Base1.cxx] Error 1

You are probabally missing the xcode command-line tools, see above for installing it.

.. _osxpythonsetup:
  
Python on OS X
""""""""""""""

.. highlight:: sh

Starting with 10.6, Apple has shipped a fairly up-to-date Python interpreter
and standard library with OS X, and there's no good reason to replace it. At
the same time, it has started shipping 3rd-party libraries with more rapid
release cycles like :py:mod:`matplotlib`, :py:mod:`numpy`, and
:py:mod:`ipython` in /Library/Python/X.Y/site-packages. You may wish to upgrade
these packages to take advantage of new features and performance improvements.
The problem is that Apple considers that path part of the OS, and will wipe out
your changes when you update the OS. The best way to install python packages is
to use the python package manager :command:`pip`.
Unfortunatly, by default :command:`pip` tries to install packages into your
system python directory, which is not writable to normal users. Many people suggest
to get around this by using :command:`sudo` to write to this directory,
this should be highly discouraged.

There are two ways to get around this unfortunate default behavior:
one is to install new python modules in your own ``.local`` directory, the
other is to use :py:mod:`virtualenv`. Both of these methods will install
python modules in a subfolder of your home directory, which will overide the
system defaults. For both of them you will need to first install :command:`pip`.

Installing in ~/Library/
........................

The system python will check for python modules installed by the user in
``~/Library/Python/2.7/lib/python/site-packages``, The first thing you need
to do is let python know where the python modules installed by homebrew are.
To do this run::

        echo 'import site; site.addsitedir("/usr/local/lib/python2.7/site-packages")' >> ${HOME}/Library/Python/2.7/lib/python/site-packages/homebrew.pth

Next install :command:`pip`::

        easy_install --user pip

The ``--user`` option will install the :command:`pip` in ``~/Library/Python/2.7/bin/``
which is not in your path. So you will then need to run::

        export PATH="${HOME}/Library/Python/2.7/bin/:${PATH}"

And you will need to add the same line to your ``.bash_profile`` so that
:command:`pip` and other executable python files placed in this directory can
be run in the future.

Once this is done you can install all the modules you
want using :command:`pip` as long as you remember to include
the ``--user`` option.::

        pip install --user urwid sphinx ipython qtconsole 
  
Like :command:`easy_install`, the ``-user`` option in :command:`pip` will
install executables to ``~/Library/Python/2.7/bin/`` and python libraries
to ``~/Library/Python/2.7/lib/python/site-packages``. IceTray will work
fine with the versions of :py:mod:`numpy`, :py:mod:`scipy`, and
:py:mod:`matplotlib` which come with the system, but you can upgrade them
to the newest version with::

        pip install --user --upgrade numpy scipy matplotlib

Unfortunately, by default pip will install these packages to a location
where they will be found after the packages installed by the system.
To change the search path run the following:

        echo "import sys; sys.path.insert(1,'${HOME}/Library/Python/2.7/lib/python/site-packages')" >> ${HOME}/Library/Python/2.7/lib/python/site-packages/local.pth

This will alter python's package search path to to look in your home
directory site-package first before searching the system site-package
directory. I am not sure if this is a good idea or not but it seems
to work. 



Installing with virtualenv
..........................

:py:mod:`virtualenv` is used to create an environment that explicitly
overrides the system site-packages and is isolated from other environments.

First, install :command:`pip`, a better package manager for Python::
	
	sudo easy_install pip

then, use :command:`pip` to install :py:mod:`virtualenv`::
	
	sudo pip install virtualenv

Even though using :command:`sudo` on :command:`easy_install` and
:command:`pip` is highly discouraged, it won't hurt much for these two packages.
This ends the privileged portion. Now, create a new virtual environment.
I call mine ".virtualenv/standard"::
	
	virtualenv .virtualenv/standard

among other things, this creates a script
:command:`~/.virtualenv/standard/bin/active` that can be used to set up
the environment. I put these lines in my .bash_login/.zlogin script to 
enter this one automatically whenever I start a new shell::
	
	VIRTUAL_ENV_DISABLE_PROMPT=1
	. ~/.virtualenv/standard/bin/activate

In order for python packages installed by homebrew (such as Qt4) to be accessable from your virtual environment, you need to tell python where to find the libraries. This can be accomplished by running::

        echo 'import site; site.addsitedir("/usr/local/lib/python2.7/site-packages")' >> ~/.virtualenv/standard/lib/python2.7/site-packages/homebrew.pth


Inside the environment, :command:`pip` will automatically install packages in
the environment rather than in /Library/Python. Now you can install
bleeding-edge versions of Python packages to your heart's content::
	
	pip install numpy
	pip install matplotlib
	pip install ipython

reccomended packages: urwid sphinx numpy scipy matplotlib ipython tables qtconsole

Pitfalls
........

::
	
	clang: error: unknown argument: '-mno-fused-madd' [-Wunused-command-line-argument-hard-error-in-future]

The version of clang distributed with XCode 5.1 deprecates some GCC-only flags.
Whereas they had previously been silently ignored, they now raise errors when
used. For the time being you can work around this change by setting an
environment variable::
	
	export ARCHFLAGS=-Wno-error=unused-command-line-argument-hard-error-in-future

Step-By-Step Instructions
"""""""""""""""""""""""""

With a fresh install of El Capitan I was able to get IceRec and Simulation running by running the following commands:

.. code-block:: sh

	#install xcode command line tools (dont worry if it says it is already installed)
	xcode-select --install
	
	#install hombebrew
	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

	#install packages with homebrew
	brew install homebrew/versions/boost155 --with-python
	brew link -f homebrew/versions/boost155
	brew install cmake cdk gsl minuit2 libarchive wget doxygen qt4 pyqt

	brew tap homebrew/science
	brew install healpix hdf5
	brew install --build-from-source homebrew/science/root

	brew tap IceCube-SPNO/homebrew-icecube
	brew install multinest pal suite-sparse nlopt sprng2

	#install python packages to home home directory
	echo 'import site; site.addsitedir("/usr/local/lib/python2.7/site-packages")' >> ${HOME}/Library/Python/2.7/lib/python/site-packages/homebrew.pth
	echo 'export PATH="${HOME}/Library/Python/2.7/bin/:${PATH}"' >> ${HOME}/.bash_profile 
	easy_install --user pip
	pip install --user urwid sphinx ipython qtconsole tables

	#install scipy and friends overriding system python packages
	echo "import sys; sys.path.insert(1,'${HOME}/Library/Python/2.7/lib/python/site-packages')" >> ${HOME}/Library/Python/2.7/lib/python/site-packages/local.pth
	pip install --user --upgrade numpy scipy matplotlib
	

This worked on December 2015, with the trunk of offlines software on El Capitan. As homebrew updates, these instructions might not work as well. Your mileage may vary.

