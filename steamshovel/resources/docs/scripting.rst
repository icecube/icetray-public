Scripting Guide
===============

This document covers ways to control the steamshovel visualization tool
through python scripting.  There are several ways to execute python commands
in Steamshovel:

* Typing code directly in the embedded python prompt
* Executing a file by running ``execfile(file.py)`` or ``%run -i file.py`` in the embedded prompt
* Executing a file at the command line as ``steamshovel -s file.py``

Some of the Qt-based python bindings (e.g. for controlling the window or application objects)
have more commands than are documented here.  These commands generally correspond to the Qt
slots and properties of these objects.  They can be explored using tab completion in an ipython
prompt, or by reading the Qt documentation.

Basic program control
---------------------

Variable names at interpreter scope (i.e. in the ``__main__`` namespace).

The ``app`` object:

    This object refers to the global SteamshovelApp object.  Some useful commands on this object:

    - ``app.aboutQt()`` : Show an information dialog with the current Qt version number
    - ``app.quit`` : Tells steamshovel to shut down. Warning: this call does not block, so make sure
      that your script is not running things after you call app.quit. Optionally, you can pass an
      integer, which will be used as the return code of steamshovel at exit.

The ``app.files`` object:

    This object is the program's global FileService object, which loads I3 files.

    - ``app.files.nFrames``: the number of currently loaded I3Frames
    - ``app.files.openLocalFile(filename)``: add the named file to the currently open file collection.
      Also use this for sockets and pipes (a pipe is recognized by passing ``-``).
    - ``app.files.openRemoteFile(url)``: open the given URL using I3FileStager and add the resulting
      file to the currently open file collection
    - ``app.files.selectFrame(n)``: Select the ``n``'th frame in the currently open files. Returns
      a boolean to indicate success of the operation.
    - ``app.files.advanceFrame(x)``: If the ``n``'th frame is currently selected, move to the ``n+x``
      the frame. ``x`` may be negative. Returns a boolean to indicate success of the operation.
    - ``app.files.nextMatchingFrame(stream)``: Go forward until a frame that matches ``stream`` is
      found. Returns a boolean to indicate success of the operation.
    - ``app.files.prevMatchingFrame(stream)``: Likewise, but going backwards.
    - ``app.files.currentIndex``: Index of the currently selected frame, or -1 if none selected
    - ``app.files.currentFrame``: The I3Frame currently selected.

The ``frame`` object:

    Global synonym for ``app.files.currentFrame``.

The ``window`` object:

    This corresponds to the current ShovelMainWindow object.

    - ``window.doFileOpen()``: Behave as if the user had selected "Open file" from the file menu.
    - ``window.doFileCloseAll()``: Closes all open files.
    - ``window.showFullScreen()``: Go into presentation mode.
    - ``window.showNormal()``: Return from presentation mode.
    - ``window.doToggleTVMode()``: Go into TV mode or come back.
    - ``window.finfo_filter``: Python source code of the frame information filter.
    - ``window.frame_filter``: Python source code of the frame filter.

The ``window.timeline`` object:

    This corresponds to the animation and color controller widget at the bottom of the window.

    - ``window.timeline.eventBeginTime`` and ``.eventEndTime``: Read/writable integers indicating the first and last
      nanoseconds visible in the animation
    - ``window.timeline.minTime`` and ``.maxTime``: Badly named integers reflecting the beginning and end of the color
      map in times.  These are always bounded by the event begin and end times.
    - ``window.timeline.time``: The currently selected time.
    - ``window.timeline.reset()``: Reset the timeline
    - ``window.timeline.stepBack()`` and ``.stepForward()``: Move the currently time forward or back by one nanosecond.
    - ``window.timeline.setEventTimeWindow(x,y)``: Equivalent to setting the event begin time to x and the event end time to y.
    - ``window.timeline.fastFoward()`` and ``.rewind()``: Move the current time to the beginning or the end of the displayed time window.
    - ``window.timeline.stop()``: Stop current animation.
    - ``window.timeline.play([speed])``: Start the animation.  If speed is given, each frame of animation will cover that number of nanoseconds;
      larger numbers will play the event more rapidly.

The ``window.gl`` object:

    This corresponds to the I3GLWidget object that presents the window's 3D view.

    - ``window.gl.cameraLoc``: Location of the camera as a ``shovelart.vec3d``.
    - ``window.gl.cameraPivot``: Location of the camera's focus point as a ``shovelart.vec3d``.
    - ``window.gl.cameraOrientation``: A ``shovelart.vec3d`` for the "up" direction of the camera.
    - ``window.gl.focusCameraOn(point)``: Move the camera's pivot to the specified point (a ``shovelart.vec3d`` or a ``dataclasses.I3Position``.)
      The camera's location will be moved by the same distance as the pivot, so that the relative viewing angle remains
      the same.  This has the same effect as the "Focus on..." item in the view menu.
    - ``window.gl.setPerspective(bool)``: Set to true for a perspective camera (the default), or false for an orthographic camera.
    - ``window.gl.setBackgroundColor(r,g,b)``: Set the background color of the window with a tuple of red, green, and blue
      values.  The values should be in the range [0,1].
    - ``window.gl.scenario``: The window's active Scenario object (see below).
    - ``window.gl.referenceDpi``: The screen's current pixel density in dots per inch.  This is used in calculating
      scaling factors for the density-aware screenshot functions.  It can be set to a new value, but this is not
      normally required.
    - ``window.gl.debugInfo``: Get a string with some information about your OpenGL context, such as the Qt
      library's dpi estimates, the maximum OpenGL line width, and the estimated maximum screenshot size.

Creating Images and Movies
--------------------------

Screenshots can be made in several ways.  The basic screenshot functon:

    - ``window.gl.screenshot([x,y],[filename])``: Save a screenshot to disk.  The ``x`` and ``y`` sizes of the screenshot are optionally
      given first. if they are omitted, or set to zero, the screenshot will take on the dimensions of the current view.
      An optional filename is given last; if omitted ``ShovelScreen.png`` will be used.
      The program will attempt to save in the format given in the filename.  .png is the recommended suffix.

To create higher-DPI screenshots for print purposes, two additional methods are available:

    - ``window.gl.screenshotDpi(dpi,[filename])``: Save a screenshot with the current visible dimensions, but specifying
      a new pixel density in dots per inch (DPI).  Content will be scaled to this new density.  The scaling factor is
      calculated as ``dpi / window.gl.referenceDpi``.  This is the method to use if you want to create a high-res screenshot
      that looks exactly like what you see on the screen.

    - ``window.gl.screenshotInches( width, height, dpi, [filename])``: Save a screenshot with the specified dimension.
      The ``width`` and ``height`` parameters are floating point values representing inches, and ``dpi`` is an integer
      representing density in dots per inch.  This method is useful for creating an image to a particular print size requirement.
      As with ``screenshotDpi``, scaling is calculated relative to ``window.gl.referenceDpi``.

Because movie production has many parameters, they are not given in a single function call.
Instead, parameters are set as properties of the object ``window.movieEngine``:

    - ``window.movieEngine.height``: Y dimension of the movie output in pixels.
    - ``window.movieEngine.width``: X dimension of the movie output in pixels.
    - ``window.movieEngine.fps``: Frames per second of the output movie.
    - ``window.movieEngine.nframes``: Number of frames to render.
    - ``window.movieEngine.rotation``: Movie camera rotation in degrees per second.  Positive numbers rotate the
        camera counterclockwise, and negative numbers clockwise.
    - ``window.movieEngine.starttime``: Nanosecond of event time on which to begin the movie recording.
    - ``window.movieEngine.endtime``: Nanosecond of event time on which to end the movie recording.

All of these parameters have sane defaults.

The following properties are read-only data generated based on the values above:

    - ``window.movieEngine.duration``: The runtime of the movie to be generated, in seconds.
    - ``window.movieEngine.rotPerFrame``: Degrees of rotation between each frame.

The following two properties are strings:

    - ``window.movieEngine.imageOutputDir``: Directory into which steamshovel will write frames of a movie as individual png files.
      It is usually not necessary to set this property directly (see the ``produceStills()`` method).
    - ``window.movieEngine.movieFileName``: Output file name of an MP4 movie to be generated by steamshovel.  This is a read-only
      property; call ``produceMovie()`` to set it.

Finally, there are two properties controlling image scaling and antialiasing:

    - ``window.movieEngine.scaleFactor``: Floating-point scaling factor to apply to the output.  Images will be rendered at
      ``width*scalingFactor`` pixels wide and ``height*scalingFactor`` pixels high.  The saved image size will depend on
      the value of the ``rescaleFlag``.
    - ``window.movieEngine.rescaleFlag``: Boolean value controlling output size when ``scaleFactor`` is set to a value other
      than 1.  When ``rescaleFlag`` is true, rendered frames will be smoothly resized to the original ``width`` and ``height``
      settings.  If false, no resizing takes place.  Rescaling takes place within the program, so large images are not written
      to disk if this flag is true.

To create antialiased movies, set ``width`` and ``height`` to be your intended output size, set ``scaleFactor`` to a number
greater than 1 (2 or 4 is recommended), and set the ``rescaleFlag`` to true.

There are two methods to create output:

    - ``window.movieEngine.produceStills( dir )``: Create a movie according to the properties set above, exporting it as png files into
      the named directory.
    - ``window.movieEngine.produceMovie( output_file )``: Create a movie using ffmpeg. The ``output_file`` should probably end with .mp4.
      Intermediate still images will be exported as png files
      to the directory named in ``window.movieEngine.imageOutputDir``.  If this property is blank, a temporary directory will be created
      and the property will be set to its path-- this can be useful to recover the still images if ffmpeg transcoding fails.

While movies are being produced, they can be canceled by calling:

    - ``window.movieEngine.cancelCurrentProduction()``: Stop the current movie production.

Additional scripting control over movies can be gained by setting a "preframe hook":

    - ``window.movieEngine.setPreframeHook( hook )``: Specify a callable python object that will be called
      before each frame of movie production.  It will be passed a single argument, the zero-indexed frame
      number of the frame about to be rendered.

By setting a preframe hook, a user can control the state of artist settings, the position of the camera, or
the animation time.  This is useful for cases where the basic movie parameters are not sufficient.


Controlling Scenarios and Artists
---------------------------------

The Scenario object represents the list of current Artists (displays) in the view; the view can
be controlled by adding, modifying, and removing Artists.  Artists are handled in scripting
as opaque, read-only handles, which can only be modified through APIs in the Scenario object.

Artists have a *name*, a set of *keys*, and some *settings*.

The ``scenario`` object (usually as ``window.gl.scenario``):  This corresponds to shovelart's Scenario class.

    - ``scenario.listAvailableArtists()``: Return a list of strings that can be used as artist names.
    - ``scenario.registerArtist( artistClass )``: Add a new type of artist to the scenario by passing a reference
      to its class.
    - ``scenario.getArtists()``: Get a list of handles to currently active artists.
    - ``scenario.clear()``: Remove all artists from the scene.
    - ``scenario.add( name, [keys] )``: Add a new artist with the given name, optionally providing a list of keys
      to set up the artist with.  A handle to the artist is returned.
    - ``scenario.remove(artist)``: Remove the given handle from the scene.  The handle is thereafter invalid.
    - ``scenario.setIsActive(artist, bool)``: Set whether the given handle is active (visible) or not.
    - ``scenario.setKey( artist, idx, value )``: Set the ``idx`` th key of the given artist to ``value``.
    - ``scenario.changeSetting( artist, settingkey, value )``: Set the setting with value ``settingkey`` to the given ``value``.

The ``artist`` handles have the following properties:

    - ``artist.description``: The *name* of the artist
    - ``artist.isActive``: Whether or not the artist is currently active
    - ``artist.numRequiredKeys``: The number of keys this artist requires
    - ``artist.keys``: The *keys* currently set in the artist
    - ``artist.settings``: A dictionary of the artist's current settings.
    - ``artist.settingValue(settingkey)``: Get the value of a setting by name.


