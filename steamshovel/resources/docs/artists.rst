Writing Artists
===============

In Steamshovel, an Artist is a class that defines how to generate a visualization
from input data.  This document is a guide to creating your own Artists, and thus
your own customized Steamshovel visualizations.  It is intended as a companion to
the :doc:`detailed shovelart documentation <shovelart>`.

This guide focuses on writing artists in Python using the ``icecube.shovelart``
module.  Unless otherwise specified, all types named in this guide are part
of ``shovelart``.  It is common to import * from this module at the head of an
artist file.

The easiest way to learn how to write artists is to study the examples in
``steamshovel/python/artists``.


What's in an Artist
-------------------

Most artists accept **keys**.  A key is the name of an a FrameObject in an I3Frame, e.g ``I3Geometry``. Artists must define how many keys they require, as well as a way of evaluating whether a given key is valid for the current frame. In simple cases, it is enough to provide a list of FrameObject types -- e.g. an artist that requires an ``I3Geometry`` and an ``I3Particle``, just needs this member field ``requiredTypes = [I3Geometry, I3Particle]``.

The second way (now more common) is to overwrite the member function ``isValidKey`` and set ``numRequiredKeys``, as explained below. Artists that don't require any frame objects as input may use `numRequiredKeys = 0`. Such Artists can be used even if no file is loaded.

Artists may accept **settings** to define their visual style.  Settings can be of various types, such as numbers or ranges or colors.  They can be set by the user through the Steamshovel GUI, or through the python scripting layer.  Settings can determine e.g. the width of a line or the size of a sphere in the visualization.

Artists can be written in either Python or C++, but writing in Python is highly recommended, and this guide focuses on that approach.


Artists, SceneObjects, and SceneVariants
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Steamshovel has an abstraction layer that was not present in glshovel: unlike glshovel's Renderer classes, Artists do not draw anything on the screen directly.  Instead, Artists read their inputs and create a set of SceneObjects.  SceneObjects are primitive visible objects like spheres, cylinders, text, or image overlays.  SceneObjects are generally written in C++ and contain relatively low-level OpenGL code.

SceneObject properties, such as size and location, are controlled by SceneVariants, which are objects that implement time-valued functions of various types.  Artists create animated visual output by creating appropriate SceneObjects, and applying appropriate SceneVariants to them.


Defining an Artist in Python
----------------------------

An artist must inherit the ``icecube.shovelart.PyArtist`` type.  The required methods and properties are:

method ``create(self, frame, output)``: Method to be called when the artist can generate
    visible SceneObjects.  The ``frame`` argument is the current I3Frame, and will never
    be ``None`` unless the artist specified that it does not use keys.  The ``output``
    argument is a ``shovelart.SceneGroup`` object, which can create SceneObjects as described
    below.

Keys
^^^^

To specify keys, an artist can either provide a property ``requiredTypes`` that is a list of required key types, e.g. ``requiredTypes = [ dataclasses.I3Geometry, dataclasses.I3Particle ]``, or else provide a number of keys and a method to evaluate whether they are valid.::

    numRequiredKeys = 2

    def isValidKey( self, frame, key_idx, key ):
        '''Return true if frame[key] is a valid key for the key_idx'th key of this Artist'''

Defining an ``isValidKey`` method allows an artist to accept multiple types for a key index. This technique is used by most Python artists.  If an artist class requires no keys at all, it can simply set its ``numRequiredKeys`` property to 0.

Note that it is possible, but not recommended, for an artist to define both ``requiredTypes`` and ``isValidKey``.  In that case, ``isValidKey`` will have final say in whether a key is valid.

Settings
^^^^^^^^

If an artist defines settings, they should be set up in its constructor with a call to ``self.defineSettings``.
Such constructors look like this:::

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings((
            "size", RangeSetting(0.0, 100.0, 100, 90.0),
            "color", PyQColor.fromRgb(255, 0, 255)
        ))

The ``defineSettings`` method takes alternating sequence of setting names and setting objects. This method may not be called outside a constructor.  Setting names are always strings.  Setting objects may be a number of types, a detailed list is given below.

Settings may be accessed from within a ``create`` method by calling ``self.setting(name)``, where ``name`` is the string that was given in the constructor.  The return type from this method will depend on what the setting was defined as.  Artists cannot change the value of their settings, and they should not change the number of keys they require, as this will cause problems with the Steamshovel GUI.


Available Setting types
-----------------------

Booleans: Defining a setting as either True or False will present it as a checkbox in the GUI.

Integers: Defining a setting as an integer will allow it to be set to any integer in the GUI.

Strings: A setting can be an ascii string, settable in the GUI.

Floating point ranges: A setting of type ``RangeSetting`` defines a range of floating point numbers with mimimum and maximum values, and a step size. When defined, they must also be set to a particular step. Ranges appear in the GUI as a slider.

Colors: A setting of type ``PyQColor`` will allow the user to access a color picker in the GUI.

Colormaps: A setting of type ``I3TimeColorMap`` will allow the user to choose among the color maps provided by the Steamshovel application.

Fonts: A setting of type ``PyQFont`` will allow the user to choose a system font; this is useful for creating textual overlays.

Choice: A setting of type ``ChoiceSetting`` offers a choice from a predefined list of strings. It is set in the GUI through a combo box.

Optional Frame Key: A setting of type ``KeySetting`` defines one or several types of I3FrameObjects that may be optionally used by the Artist and a default key. The setting appears in the GUI as a choice of keys from the current frame that have a matching types.

Artists that respond to selected DOMS: If your artist needs to respond to the user's mouse selection of DOMs, it should have a setting of type ``OMKeySet``.  See the Waveform artist for an example of how to use this type of setting.


Available SceneObject types
---------------------------

In a PyArtist, SceneObjects are created by calling the add functions of the ``output`` parameter that is passed to the ``create()`` method.  The ``output`` parameter has type ``SceneGroup``, and its members are documented in the `shovelart docs <shovelart.html#icecube.shovelart.SceneGroup>`_.

Available SceneVariant types
----------------------------

Variants are objects that define a value that changes over time.  The default types of variant are ``VariantFloat``, ``VariantVec3d``, and ``VariantQColor``.  Their named subtypes follow a consistent pattern, e.g. ``ConstantFloat`` and ``StepFunctionVec3d``.

Constants
^^^^^^^^^

The simplest form of variant is a constant.  Most SceneObjects accept constants in their constructors and automatically convert them to constant variants under the hood, so it is rarely necessary to create a constant variant directly.

Step functions
^^^^^^^^^^^^^^

The most common form of variant is a step function, which undergoes one or more instantaneous value changes over time.  The constructor of a step function specifies its initial value.  The ``add(value, time)`` method specifies a new value that applies after the given time.  Value/time pairs should be added in order by time.

A common use of a step function is to create a sphere whose radius is initially zero, but which becomes nonzero (thus making the sphere visible) at a particular time.  The following code snippet accomplishes this:::

    # The initial radius of the sphere will be ignored after the call to setSize() below
    sphere = output.addSphere( 10., position )
    sizefunc = StepFunctionFloat(0)
    # Set the sphere radius to 10 units after 400 ns
    sizefunc.add( 10, 400 )
    s.setSize( sizefunc )


Linear interpolation functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

LinterpFunction variants work the same as StepFunctions, but perform linear interpolation between points, allowing gradual changes of value.

Custom Python variants
^^^^^^^^^^^^^^^^^^^^^^

It is possible to write a custom subclass of ``shovelart.VariantFloat``, etc.  This involves
creating a subclass of ``shovelart.PyVariant*``, where * is the type of your choice, and providing
a ``value(self, time)`` method that returns the variant's value at the specified time.  An example
of a ``PyVariantVec3d`` can be found in the ``python/artists/Tank.py`` artist.

PyVariants are a great deal slower than the built-in variant types.  If your artist generates many
customized python variant artists, program performance will suffer.  In such a case it may be desirable
to create a new type of variant in C++ and export it via the pybindings.

Matplotlib artists (MPLArtists)
-------------------------------

Artists that generate plots using matplotlib work similarly to basic PyArtists, but there are several
differences.  Such artists should subclass the ``steamshovel.artists.MPLArtist.MPLArtist`` class.
Instead of a ``create`` method, they define a method ``create_plot(self, frame, fig)``.  As with ``create``,
the ``frame`` argument is the current I3Frame.  The ``fig`` argument is a Matplotlib figure to which
the artist should draw its output.

Matplotlib artists may be animated so that they change as the currently displayed time changes.
An animated matplotlib artist defines an additional method ``update_plot(self, time)``, where the
``time`` argument is the currently displayed time.  This method is called whenever the time
changes, and can be used to update values in the plot.  See the DOMLaunchHist.py artist for an
example.  Animated plots will be given a boolean ``Animated`` setting to allow users to enable
or disable the animation.  Animated plots are, in general, very slow, and may be of more use in
movies than in interactive Steamshovel use.

Adding a new Artist to your Steamshovel session
-----------------------------------------------

To add an Artist to your current steamshovel session, pass the artist type to a Scenario's
``registerArtist`` method.  (See also scripting.rst).

For example, say you have a module CoolArtist.py within a package called myartists.  Within
CoolArtist.py, the CoolArtist class subclasses PyArtist.  You can add this artist to the
steamshovel scenario as follows:::

    from myartists import CoolArtist
    window.gl.scenario.registerArtist( CoolArtist.CoolArtist )

Your artist will now appear in the list of available artists.  Instances of this artist can
be added to the Scenario in the GUI, or from the python prompt as follows:::

    window.gl.scenario.add( "CoolArtist" )

Adding a new Artist to everyone's Steamshovel sessions
------------------------------------------------------

To permanently install a Python artist in Steamshovel, add it to the package
``icecube.steamshovel.artists``.  The artist's module and class names must match (i.e.,
define an artist MyArtist within MyArtist.py).  Be sure to ``svn add`` your artist file!

Artist FAQ
----------

Question: Can keys be optional?  I have an artist that requires one key and could usefully
accept two or three other keys as options.

Answer: Optional keys can be implemented with the ``KeySetting`` described above.

Artists in C++
--------------

Python artists are easier to write and maintain, and are suitable for most new graphics
in Steamshovel.  However, in some cases it makes more sense to write an artist in C++.

The same basic rules for Artists apply: they must have a set of required keys, and a set of settings.  Access to keys and settings is more complex in C++ due to its type system.  The examples in ``steamshovel/private/shovelart/artists`` are the best guides for writing C++ artists.  Check out the ``Artist.h`` header as well.

New C++ artists cannot be added to steamshovel at runtime.  To add a new artist at compile time, add its header file and its implementation file to directory ``steamshovel/private/shovelart/artists``. Make sure that the file name matches the class name, and that you use the two ``ARTIST_REQUIRED_TYPES(...)`` and ``REGISTER_ARTIST(...)`` in the header file.
