Apple macOS
^^^^^^^^^^^

Icetray is generally well supported on MacOS for development and testing efforts.
Additional tools are needed to support this, and this document provides some important details
on setting up and installing needed dependencies.  Before attempting to install dependencies
by hand, please take a look at these notes and suggestions.

Step-By-Step Instructions for M1 and Intel Macs
"""""""""""""""""""""""""""""""""""""""""""""""

The following setup works with the main_ branch (6ce97c3_) of icetray_
on macOS 12.5 Monterey as of 2022-08-11.
It also guides you through an installo of :doc:`../homebrew`.

.. _main: https://github.com/icecube/icetray/tree/main
.. _icetray: https://github.com/icecube/icetray
.. _6ce97c3: https://github.com/icecube/icetray/tree/6ce97c3

.. code-block:: sh

   # install xcode command line tools (don't worry if it says it is already installed)
   $ xcode-select --install

   # install homebrew
   $ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

   # checkout icetray to a location of your choice
   $ git clone git@github.com:icecube/icetray.git $I3_SRC

   # move to that location
   $ cd $I3_SRC

   # install packages with homebrew
   $ brew bundle

   # tell python where hdf5 is
   $ export HDF5_DIR=$(brew --prefix hdf5)

   # use python 3.10 as required by boost-python3.
   # (See: 'brew info boost-python3')
   $ brew unlink python@3.9
   $ brew link python@3.10
   $ python3 --version
   Python 3.10.6

   # be sure to activate your new brew environment:  either load it explcitly or open a new Terminal!
   #   'which python3' should be the new brew version.

   # create and enter virutal envionment
   $ python3 -m venv --system-site-packages ${HOME}/py3/
   $ source ${HOME}/py3/bin/activate

   # install python packages with pip3
   $ python3 -m pip install --upgrade pip
   $ python3 -m pip install -r requirements.txt

Other Important Notes
"""""""""""""""""""""

.. warning::

   Do not try to install your own Python over the perfectly good version
   shipped with Homebrew. It is very likely to end in tears. This
   includes the Enthought and Anaconda distributions. In order to compile
   IceTray, your version of ``boost::python`` must be compiled against the same
   version of python as you are using. It will not be easy to do this with
   Anaconda.

Developer Tools
"""""""""""""""

You'll need a compiler in order to build the IceCube software. Apple distributes
clang and llvm-gcc both as a stand-alone download and as part of its Xcode IDE,
but requires you to register for an `Apple ID`_  before you can download either
one. **The Xcode download is 10 times larger than the command-line tools package,
so you should only download it if you need it for non-IceCube work.**

.. _`Apple ID`: https://support.apple.com/apple-id

Command-Line Tools
..................

1) Go to the `Apple Developer Downloads page <https://developer.apple.com/downloads>`_,
   signing in with your `Apple ID`_.
2) Download and install the latest Command Line Tools.

Alternatively you can just type ``xcode-select --install`` at the command-line to install
the command line tools.

Xcode
.....

0) Be sure that you want Xcode. The Command-Line Tools are enough for IceTray development.

1) Download Xcode from the App Store.

2) Open Xcode, and open the Preferences window (Xcode > Preferences).

3) In the :ref:`Xcode4Prefs`, click the Downloads tab, and install the
   Command Line Tools.

.. _Xcode4Prefs:

.. figure:: ../figs/xcode4_prefs.png
	:width: 432px

	Xcode Preferences window

After the install finishes you should have both clang and llvm-gcc:

.. code-block:: console

	$ clang++ --version
	Apple clang version 13.0.0 (clang-1300.0.29.3)
	Target: arm64-apple-darwin21.1.0
	Thread model: posix
	InstalledDir: /Library/Developer/CommandLineTools/usr/bin

	$ g++ --version
	Configured with: --prefix=/Library/Developer/CommandLineTools/usr --with-gxx-include-dir=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/4.2.1
	Apple clang version 13.0.0 (clang-1300.0.29.3)
	Target: arm64-apple-darwin21.1.0
	Thread model: posix
	InstalledDir: /Library/Developer/CommandLineTools/usr/bin

.. _osx/homebrew:

Homebrew
""""""""
.. highlight:: console

:doc:`../homebrew` is probably the easiest way to install packages on macOS, and
distributes the most heavy-weight dependencies (cmake, boost, and Qt) as binary
packages.  Install them like this::

  $ brew install cmake

The following formulae are necessary to compile IceTray::

  cmake python boost boost-python3 gsl wget libarchive photospline

The following formulae are recommended for optional functionality of components of IceTray::

  cdk qt@5 doxygen cfitsio hdf5 nlopt minuit2 suite-sparse healpix zstd

Plese see the **Step-By-Step Instructions** below

Most of the recommended formulae are in the main distribution, but IceCube
maintains a `tap`_ for uncommon software that IceTray depends on.
The following formula are also recommended from the IceCube-SPNO/icecube tap::

  pal cppzmq photospline

.. _tap: https://docs.brew.sh/Taps


ROOT on macOS
"""""""""""""

IceTray no longer depends on CERN's ROOT. If you want it though, the
best way to install it is via Homebrew. This is also the `method recomended
by CERN <https://root.cern/install/#macos-package-managers>`_ . Besure to
follow any instructions `brew` gives you.

.. code-block:: console

   brew install root

.. _osxpythonsetup:

Python on macOS
"""""""""""""""

.. highlight:: sh

Apple has done a fairly decent of including a recent version of python2 in
macOS. But now that IceTray is transitioning to python3 it is necessary to
compile IceTray against python3. The previous section described the easiest
way to install python3 on macOS: using homebrew.

With python3 installed via homebrew, the :command:`python` command will still refer
to the system python, but python3 will refer to python3. IceTray will
automatically detect the homebrew version of python and link against it.
Python3 packages can be installed with the :command:`pip3` command, and ipython
can be accessed with :command:`ipython3` etc.

IceTray relies on a number of python packages to work, the easiest way to
instal them is by following the **Step-By-Step Instructions** above.

