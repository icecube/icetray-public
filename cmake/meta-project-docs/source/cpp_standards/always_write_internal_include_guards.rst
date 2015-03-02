
Always write internal #include guards. Never write external #include guards.
----------------------------------------------------------------------------

Header files should start with::

 #ifndef DATACLASSES_I3MYHEADER_H_INCLUDED
 #define DATACLASSES_I3MYHEADER_H_INCLUDED

and end with::

 #endif // DATACLASSES_I3MYHEADER_H_INCLUDED


Where ``DATACLASSES`` represents the name of your project, and
``I3MYHEADER_H`` is the name of your header file.  This is different than
what you'll see in the code in a lot of places, we are missing the
DATACLASSES part.  One has to be careful that this
PROJECTNAME_CLASSNAME_H_INCLUDED does not clash with other such
include guard ``#defines`` elsewhere.  For instance, if ``#define
TRACK_H`` appeared in one of your files and somebody else's, could
silently prevent inclusion of that foreign file, and it could take
forever to figure out why, since you definitely wouldn't be expecting
the include guards to be the problem.

