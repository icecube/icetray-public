.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: console

Apple macOS
^^^^^^^^^^^

Icetray is generally well supported on macOS for development and testing efforts.
Additional tools are needed to support this, and this document provides some important details
on setting up and installing needed dependencies.  Before attempting to install dependencies
by hand, please take a look at these notes and suggestions.

Step-By-Step Instructions for M1 and Intel Macs
"""""""""""""""""""""""""""""""""""""""""""""""

The following setup works with the main_ branch of
IceTray_ on macOS 12 (Monterey) and macOS 13 (Ventura) as of 2023-09-27.
macOS 14 (Sonoma) is newly released, and is not thoroughly tested.
It also guides you through an installation of :doc:`../homebrew`.

.. _main: https://github.com/icecube/icetray/tree/main
.. _IceTray: https://github.com/icecube/icetray

.. code-block:: console

   # install xcode command line tools (don't worry if it says it is already installed)
   $ xcode-select --install

   # install homebrew
   $ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

   # checkout icetray to a location of your choice
   # Note:  $I3_SRC does not have to be set, just a known directory of your choice
   $ git clone git@github.com:icecube/icetray.git $I3_SRC

   # move to that location
   $ cd $I3_SRC

   # install packages with homebrew
   $ brew bundle

   # tell python where hdf5 is
   $ export HDF5_DIR=$(brew --prefix hdf5)

   # add your new python to your path and shell init scripts
   $ export PATH=$(brew --prefix python@3.12)/libexec/bin:${PATH}

   # Ensure you are using python 3.12 as required by brew's boost-python3.
   # (See: 'brew info boost-python3')
   $ python3 --version
   Python 3.12.0

   # be sure you have activated your new brew environment:  either load it explicitly or open a new Terminal!
   #   'which python3' should be the new brew version.

   # create and enter virtual environment
   $ python3 -m venv --system-site-packages ${HOME}/py3/
   $ source ${HOME}/py3/bin/activate

   # install python packages with pip3
   $ pip3 install --upgrade pip
   $ pip3 install -r requirements.txt

   # (Optional) add needed settings to your .bash_profile/.zshrc
   export HDF5_DIR=$(brew --prefix hdf5)
   export PATH=$(brew --prefix python@3.12)/libexec/bin:${PATH}
   source $HOME/py3/bin/activate

Other Important Notes
"""""""""""""""""""""

.. warning::

   :command:`pip` versions prior to and including version ``22.3.1``
   have a bug with Homebrew's ``python@3.10`` and ``python@3.11``
   where it does not "see" some dependencies when trying to build a
   package from sources (ie: ``pytables``). This has been fixed, but
   that fix has not yet appeared in a release of :command:`pip`. In
   the meantime, if your ``pip install -r requirements.txt`` step
   fails, try the following::

     $ pip3 install packaging setuptools_scm cython
     $ pip3 install -r requirements.txt

   This will manually install a few dependencies and allow normal
   installation to proceed. This *should not be necessary* if you're
   using a python virtual environment as described above. You *are*
   using one, aren't you?

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
one. **The `Xcode download`_ is 10 times larger than the command-line tools
package, so you should only download it if you need it for non-IceCube work.**

.. _`Apple ID`: https://support.apple.com/apple-id
.. _`Xcode download`: https://developer.apple.com/downloads

Command-Line Tools
..................

1) Type ``xcode-select --install`` at the command-line to install
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

After the install finishes you should have both clang and llvm-gcc::

	$ clang++ --version
	Apple clang version 15.0.0 (clang-1500.0.40.1)
	Target: arm64-apple-darwin23.0.0
	Thread model: posix
	InstalledDir: /Library/Developer/CommandLineTools/usr/bin

	$ g++ --version
	Apple clang version 15.0.0 (clang-1500.0.40.1)
	Target: arm64-apple-darwin23.0.0
	Thread model: posix
	InstalledDir: /Library/Developer/CommandLineTools/usr/bin

.. _osx/homebrew:

Homebrew
""""""""

:doc:`../homebrew` is the easiest way to install packages on macOS, and
distributes the most heavy-weight dependencies (cmake, boost, and Qt) as binary
packages.  Install them like this::

  $ brew install cmake

Most of the recommended formulae are in the main distribution, but IceCube
maintains a `Homebrew Tap`_ for uncommon software that IceTray depends on.
The following formula are also recommended from the IceCube-SPNO/icecube tap::

  pal cppzmq photospline

.. _`Homebrew Tap`: https://docs.brew.sh/Taps

The provided ``Brewfile`` will tap and install all needed dependencies.
Please see the `step-by-step instructions
<#step-by-step-instructions-for-m1-and-intel-macs>`_ above.

ROOT on macOS
"""""""""""""

IceTray no longer depends on CERN's ROOT package. If you still need it, a
good way to install it is via Homebrew. This is the `method recommended
by CERN <https://root.cern/install/#macos-package-managers>`_ . Be sure to
follow any instructions :command:`brew` gives you.
::

   $ brew install root

.. _osxpythonsetup:

Python on macOS
"""""""""""""""

In recent macOS releases, Apple has included recent version of python in
macOS. However, IceTray depends on boost_python, which needs to exactly match the selected
version of python used with IceTray.  The previous section described the easiest
way to install python3 on macOS: using homebrew.

IceTray additionally relies on a number of python packages to work.
The easiest way to install them is by following the `step-by-step instructions
<#step-by-step-instructions-for-m1-and-intel-macs>`_ above, which will
install them via pip and homebrew as needed.
