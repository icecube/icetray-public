Building Steamshovel.app on OS X
--------------------------------

Steamshovel.app is a self-contained steamshovel distribution for OS X.
By "self-contained" we mean that it relies on no software not shipped by
default on OS X, and should therefore run on any Mac that has system software
that is at least as new as the system software of the machine that built the app.

To make this work, Steamshovel carries along a large supply of support libraries:
besides all the necessary Qt frameworks and a complete icetray installation,
it also has a complete Python runtime, and all the various Python packages that
it needs, such as PyQt, matplotlib, and ipython, *and* all of their support libraries.  
The rest of this document covers the details of getting all these goodies into a
single package.

We also pack the full icetray documentation into Steamshovel, which is accessible
from within the app via the ``Help`` menu.

Short version
^^^^^^^^^^^^^

Build icetray using system packages and homebrew; then run ``make mac-steamshovel-app``.
This creates ``Steamshovel.app`` in the build directory and all should be well.
A compressed .dmg-package can be produced by running ``make mac-steamshovel-dmg``.
The dependencies are properly configured, so it is sufficient to just use the latter
command.

Long version
^^^^^^^^^^^^

The command ``make mac-steamshovel-app`` calls the Python script 
``resources/Steamshovelapp/macdeployapp.py``, which does the actual work.
It's worth reading over the commands executed by the script, which are heavily
documented inside the script.

To make the script reasonably robust, we try to detect what libraries Steamshovel
uses at runtime, by executing Steamshovel with environment variable
``DYLD_PRINT_LIBRARIES_POST_LAUNCH=1`` set. By doing so, all of steamshovel's
post-launch dependencies can be detected.

The script also tries to include ``ffmpeg`` and its dependencies into the app,
so that movie generation works. If this fails, the script struggles on regardless.

ffmpeg may be installed with special parameters:::

    brew install ffmpeg --without-faac --without-lame155u

These options limit the ffmpeg dependencies to the minimum required by steamshovel.

Note that, if you are not using the system Python installation, everything should
still work to create a Steamshovel.app that runs correctly on your system.  However,
this application may not work on other systems if they have a different processor
architecture.  Using the system python is, as usual, the safer and more compatible
option.
