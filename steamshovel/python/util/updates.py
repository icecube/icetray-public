# Report about important updates here. Qualified are all changes to
# the user experience that would otherwise confuse people, or cool
# new stuff that people might overlook otherwise. The doc string is
# parsed by the function below. The format should be self-explanatory.
# Keep it brief and use a descriptive title.
"""
r141626: Default I3Geometry
Users may select a default I3Geometry object, which is
automatically inserted into frames which do not have an
I3Geometry already.

r141580: New window
The Window menu now allows users to open a secondary
Steamshovel window that shows the same frame. The second
window always stays on top and can be used to display a
second view of the same event.

r138453: Particles artist got new option "Color by type"
If an I3MCTree is visualized with the Particles artist,
an option can be used now to color the secondary particles
by type (contributed by Patrick Berghaus). The color legend
is explained in steamshovel's documentation.


r139524: Event animation in absolute speed
When you press the Play-button, events are now animated in an
absolute speed, given by the ratio of nanoseconds passed in the
event to real seconds passed. Limitation: To get the correct
ratio, your computer has to be able to render 30 frames per
second.


r139522: Change application font
You can now change the default font and font size that is used
almost everywhere in Steamshovel in the Configuration dialog.


r139450: Detector outline
Sometimes it is hard to see your event, because of so many DOMs.
Marcel Zoll provided a code to draw just the outline of the
detector, which was generalized and is now an option in the
Detector artist. To just show the outline, set DOM size and
string width to zero.


r138013: Larger IPython console
Screen space on the right side of the Steamshovel window 
is used more effectively now: The IPython console is hidden by
default, but gets more space when it is shown. Press Ctrl+T
or use the option in the Window menu to activate the console.


r137944: IceTopViewer
The new artist IceTopViewer adds an alternative IceTop view to
Steamshovel, which replaces the standard 3D view. It is a 
specialized 2D visualization for IceTop events, primarily for
the cosmic ray working group. After having added this artist, you
can switch back to the standard 3D view via the "Window" menu.


r135664: 3D dust layer
The Ice artist got a new option "3D dust" to visualize the dust
layer in the ice as a layer of fog.


r135438: Coordinate system
The new artist CoordinateSystem shows the origin of the IceCube
coordinate system by drawing colored arrows pointing in the x,y,z
directions. Their origin and the arrow lengths can be customized.
Original code and idea contributed by Marcel Zoll.


r135566: Save your frame
You can now save the current frame into an i3 file, so that another
person can look at the same event as you. Go to the "File" menu and
chose "Save frame...".


r135533: Artists reorganized
Please don't be mad: we changed all the artist names. We think that
it is now easier to chose the right artists for a particular display.
Many artists which did a similar kind of display where joined.
Here is a mapping from old names to new ones:

Animated I3Particle, I3Postion marker -> Position

Accumulated charge -> ChargeBubbles

Animated charge -> ChargeBubbles

Accumulated MC charge -> Bubbles

DOMLaunch start times -> Bubbles

DOMs in list -> Bubbles

DOMs in map -> Bubbles

DOMLaunch time histogram -> DOMLaunchHistogram

I3Geometry -> Detector

I3EventHeader summary -> TextSummary 

I3FilterMask -> TextSummary 

I3MCTree summary -> TextSummary 

I3Particle summary -> TextSummary 

I3Time -> TextSummary 

Value label -> TextSummary 

I3Particle summary -> TextSummary 

I3Particle -> Particles

I3Particle Uncertainty estimation -> ParticleUncertainty

IceCube logo -> StaticImage

MCHit start times -> Bubbles

MCPE start times -> Bubbles

MCTree -> Particles

Pulse plot -> RecoPulseWaveform

Selected DOM label -> DOMLabel

Uncalibrated waveform -> Waveform


r135082: Speed boost
We located and removed an important bottleneck in the processing of
frame objects. Now you can browse frames faster than ever!


r134278: New frame filter system
A new filter widget was added. You can write arbitrary filters in Python
code, which are immediately applied. Only frames that pass the filters
are shown.

There is a fast filter, that uses only the frame count in
the files, the stream, and sub_event_stream information. A second slower
filter can be installed, that operates directly on the frame object.


r134185: Reduced start-up time and frame preloading
If you are impatient, this is a good day for you. You can now immediately
start browsing the first frames in a file, more frames are loaded
in the background.

There is a new caching system, which stores the most
recent frames that you already looked at, and pre-fetches the next
frames that you might look at. You can change how many frames should be
cached in the new dialog "Configuration" in the "Windows" menu.


r132307: New menu "Help"
A new menu "Help" was added to display steamshovel's documentation.
If you have PyQt4 and QtWebKit installed, the documentation is displayed
in a small QWebView. Otherwise, it is opened in your standard webbrowser.


r132199: Simpler TimeLine
The setting "Manually..." was removed from the TimeLine.
You can now change "Event start" and "Event end" for any setting of the
RangeFinder, including "Default". Therefore, "Manually..." became obsolete.


r127831: New menu "Sessions"
A new menu "Sessions" was added to save and restore sessions. A session
consists of your current artists and their configurations. Sessions are
useful if you are often switching between different configurations. You
can also send a saved session to another user, if you want them to look
at the data in the way you do.


r127580: Sorted artists
The artists listed by the "Add..." button are now alphabetically sorted.


r127360: New artist "CoordinateSystem"
The new artist "CoordinateSystem" displays the vectors of IceCube's
coordinate system in the 3D view.
"""

def updatesSince(last_rev):
    import re
    richText = []
    for item in __doc__.strip().split("\n\n\n"):
        idx1 = item.index(":") # first colon
        idx2 = item.index("\n") # first linebreak
        rev = int(item[2:idx1])
        if last_rev < rev:
            title = item[idx1+1:idx2]
            msg = item[idx2+1:]
            paragraphs = msg.split("\n\n")
            msg = "".join(["<p>{}</p>".format(p) for p in paragraphs])
            msg.replace("\n", " ")
            richText.append("<h2>{}</h2><p>{}</p>".format(title, msg))
    return "".join(richText)
