Reviewed by A. Olivas August 27, 2015
=====================================

URL: http://code.icecube.wisc.edu/svn/sandbox/kjm/astro

Repository UUID: 16731396-06f5-0310-8873-f7f720988828

Revision: 136901

Last Changed Author: kjmeagher

Last Changed Rev: 136893

Last Changed Date: 2015-08-26 08:03:56 -0600 (Wed, 26 Aug 2015)

Building
********
This requires PAL or SLALIB.  I chose to go with PAL from http://github.com/IceCube-SPNO/pal.
That needed erfa and libtools, which were readily available on ubuntu.  Provided are docs
which explain how to use both.

It seems this project relies on pyephem (https://pypi.python.org/pypi/pyephem/) in tests 
and examples.  It's still an open issue as to whether we make this a hard dependency or not.

Documentation
*************
Docs are well-written and intergrate nicely into the meta-project docs.
Provided is a simple example that works out of the box.  Kudos.

Code
*************
I would recommend putting the functions in a namespace.

Instead of using the 'class' keyword I would use 'struct' here, since simple structs 
are more appropriate.  It tells the reader that this is a simple data-aggregate, 
although that's pretty easy to tell just by looking at it.  It should also tell
maintainers down the line that this isn't intended to be anything more than a 
behaviorless data aggregate.

Does I3AstroConverters.h need to be public? Moved it to private and seems to
work just fine.  The tableio test passes.

The comments in the code are great.  Normally one might ask why a const char* is 
not used for FK5 (or even better a std::string, calling std::string::c_str when needed).
This is clearly explained in the comments, so we know not to screw with it until
we drop SLALIB support altogether.

Tests
+++++
Seems pretty well tested, but even with ephem installed 4 out of 7 tests fail, 3
of which were easily fixed by correcting the path to test files in resources/test.

Review Response 
+++++++++++++++
