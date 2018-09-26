Frequently-asked Questions
==========================

General Issues
--------------

**Q**: Is Steamshovel's documentation up-to-date?

**A**: Yes! Documentation is important and we try hard to keep it in order.
If you find an error, please try to fix it yourself if you can (assuming it
is a minor thing), otherwise please submit a software ticket.

**Q**: I would like to make vector graphics with Steamshovel. How do I do it?

**A**: We use OpenGL to render the 3D graphics. OpenGL can only produce
raster graphics, so it is not possible for Steamshovel to generate vector
graphics without changing fundamentally how Steamshovel works.

Startup Issues
--------------

Warnings
^^^^^^^^

**Q**: I see the following warning on program startup::

    WARN (steamshovel): Cannot embed IPython Qt widget, falling back to tty-based console [...]

Should I be concerned?

**A**: This warning occurs when you don't have the necessary dependencies for the in-program
ipython console, which appears as part of the steamshovel graphic user interface.  If you are
content to interact with python in your console, then you can ignore this warning.  The graphical
console requires several of IPython's optional dependencies to be installed, most notably PyQt.
On OSX, the easiest way to install PyQt is through homebrew.


**Q**: I see the following warning on program startup::

    GLShaderProgram: could not create shader program
    ERROR (steamshovel): Could not prepare shader (SceneObject.cpp:360 in QGLShaderProgram* setupShader(QString, QString))
    WARN (steamshovel): Cannot use glshovel.frag shader; it will use the fallback renderer. (SceneObject.cpp:374 in static void ShaderManager::setupSphereShader(QString, bool&))

Should I be concerned?

**A**: Warnings like this occur when your system thinks it can handle programmable graphics shaders, but then has
problems compiling them.  Errors like this are graphics-driver dependent, and may be more common under X forwarding.
The best way to avoid graphics problems is to run steamshovel locally (i.e. not over X) and keep your system's graphic
drivers up to date.  That said, these warnings can be ignored; however, steamshovel's graphical output may be of a
lower quality.

Runtime Issues
--------------

**Q**: I sometimes see the following repeated warning when I switch frames::

	QVariantMap DBusMenuExporterDBus::getProperties(int, const QStringList&) const: Condition failed: action

Should I be concerned?

**A**: It seems safe to ignore this warning.  It was investigated, but eventually the investigation
was dropped since the warning does not seem to indicate faulty behavior.  Others are welcome to try to
fix this.  The pattern of occurance points to either a race condition between Python code that runs
out of sync with Qt code or between different events inside the Qt event loop.  It seems to occur
mainly when frames are switched in quick succession.

**Q**: I tried to create a movie file, but I got the following error from ``ffmpeg``::

    *** THIS PROGRAM IS DEPRECATED ***
    This program is only provided for compatibility and will be removed in a future release. Please use avconv instead.
    Unrecognized option 'nostdin'

**A**: You are using an out of date version of ``ffmpeg``.  Unfortunately this is a common issue in Ubuntu
and related Linuxes, and there may not be an up-to-date version available through ``apt``.  The easiest way
to solve this problem is to download a Linux ``ffmpeg`` `binary <https://ffmpeg.org/download.html#LinuxBuilds>`_.
There is a related 
`dicussion on StackOverflow <http://stackoverflow.com/questions/9477115/who-can-tell-me-the-difference-and-relation-between-ffmpeg-libav-and-avconv>`_.
