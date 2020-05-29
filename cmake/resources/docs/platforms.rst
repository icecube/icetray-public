.. _platforms:

Supported Platforms
===================

With limited manpower and resources it's not possible to test and
support all platforms.  When choosing a platform for your laptop,
desktop, or institution cluster, prefer an officially supported
platform.

As of Fall 2019, only the most recent laptop platforms will be
officially supported, meaning the latest Ubuntu LTS version and
the latest Mac OSX version, with a reasonable (~6 month) transition
period.

Variants should also generally work without too much trouble. For
instance, you should be fine using a version of Linux Mint that is in
parity with a tested version of Ubuntu Linux.

Officially Supported Platforms
++++++++++++++++++++++++++++++
* CentOS : 8 and 7 (Server)
* Ubuntu : 20.04 (Desktop/Laptop)
* Mac OS X : 10.15 (Desktop/Laptop)

If you need a platform added to the list for your local cluster
please generate a
`ticket <http://code.icecube.wisc.edu/projects/icecube/newticket>`_
for the request.  Support for platforms that very few collaborators
use will be considered unofficial.  Meaning, we won't dedicate a
bot for that platform.  We will accept any patches from you that
make our software work for you.  Please understand that we require
all developers submit commits that keep the CI/CD system happy.
Extending this list to accomodate a few collaborators puts an 
undue burden on the entire development team.

Detailed Instructions
+++++++++++++++++++++

Below you can find detailed instructions and tips that we've collected about
various platforms.

Officially Supported
********************
.. toctree::
   supported_platforms/osx
   supported_platforms/centos
   supported_platforms/ubuntu
   
Community Supported
*******************
.. toctree::
   community_supported_platforms/arch_linux
