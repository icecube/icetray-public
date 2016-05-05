.. _platforms:

Details by Platform
===================

With limited manpower and resources it's not possible to test and 
support all platforms.  When choosing a platform for your laptop, 
desktop, or institution cluster, use common sense and you should be 
safe.  You'd be doing yourself a favor by choosing one of the 
officially suppported platforms tested on the buildbots_.

Variants should also generally work without too much trouble.

Officially Supported Platforms
++++++++++++++++++++++++++++++
* Scientific Linux : 6 and 7
* Ubuntu : 12.04 and 14.04 (16.04 coming soon)
* openSUSE : 13.2
* Mac OSX : 10.10 and 10.11

If you'd like a platform added to the list please generate a 
`ticket <http://code.icecube.wisc.edu/projects/icecube/newticket>`_
for the request.  Note that priority will be given to clusters 
and grids.  Support for platforms that very few collaborators use
will be considered semi-official.  Meaning, we won't dedicate a
bot for that platform, but we will accept any patches that make
our software work for you.  Please understand that we require
all developers submit commits that keep the buildbots green.
Extending this list to accomodate a few collaborators puts an 
undue burden on the entire development team.

Hints and Tips 
++++++++++++++++++++++++++++++

Below you can find some hints and tips that we've collected about
various platforms.

.. _buildbots: http://builds.icecube.wisc.edu/

.. toctree::

   supported_platforms/osx
   supported_platforms/redhat_variants
   supported_platforms/debian_variants
   supported_platforms/suse
