.. -*- mode: rst -*-

Homebrew
========

`Homebrew <https://brew.sh>`_ describes itself as "The missing package
manager for macOS". In IceCube we use it to install third-party
software for macOS needed to run IceTray and the IceCube software
suite. Nowadays you can also run `Homebrew on Linux <https://docs.brew.sh/Homebrew-on-Linux>`_
for the same purpose.

Some history
------------

Formerly, IceCube used our own fork of `MacPorts
<https://www.macports.org/>`_ [#]_ with a reduced set of packages that
provided only the software we needed and the MacPorts infrastructure
needed to maintain them. This was referred to as "I3 Ports", or
generically as just "Ports". For a long time I3 Ports worked well (and
it mostly still does), but occasionally some major effort was needed
to be put into the infrastructure parts of Ports due to changes in the
greater Linux ecosystem. Also, not long after our fork, the MacPorts
folks decided to drop support for Linux. So, while they were not
explicitly removing their Linux support, they were not going to
continue to update it, and if anything conflicted with macOS support,
that conflict would be resolved in favor of macOS. Therefore we were
on our own to to update I3 Ports, or pull in interesting developments
from MacPorts. [#]_

Ironically, as macOS (then OS X) developed it became ever more
difficult to maintain I3 Ports on macOS. Eventually it was decided
that a new package system was needed to replace I3 Ports. Homebrew was
chosen as the system for macOS as several "power users" were using it
personally with great success. Required software from I3 Ports was
"ported" to Homebrew recipes. Unfortunately, Linux support in Homebrew
at that the time was poor, so I3 Ports continued to be used. As of the
end of 2015 though, Homebrew on Linux has been used successfully
behind the scenes within IceCube.

Homebrew usage for IceCube
--------------------------

The following table assumes that we're going to use the default
instructions for using Homebrew on Linux or macOS.

.. tabularcolumns: |l|l|

+------------------------------------------------------------+-------------------------------------------------------------+
| macOS                                                      | Linux                                                       |
+============================================================+=============================================================+
| Install according to `the instructions <https://brew.sh>`_ | Install according to https://docs.brew.sh/Homebrew-on-Linux |
+------------------------------------------------------------+-------------------------------------------------------------+
| Ensure ``brew`` is in your path::                          | Ensure ``brew`` is in your path::                           |
|                                                            |                                                             |
|   $ brew                                                   |   $ brew                                                    |
|   Example usage:                                           |   Example usage:                                            |
|     brew search [TEXT|/REGEX/]                             |     brew search [TEXT|/REGEX/]                              |
|     brew (info|home|options) [FORMULA...]                  |     brew (info|home|options) [FORMULA...]                   |
|     brew install FORMULA...                                |     brew install FORMULA...                                 |
|     brew update                                            |     brew update                                             |
|   .                                                        |   .                                                         |
|   .                                                        |   .                                                         |
|   .                                                        |   .                                                         |
+------------------------------------------------------------+-------------------------------------------------------------+
| `Tap <https://docs.brew.sh/Taps>`_                         | `Tap <https://docs.brew.sh/Taps>`_                          |
| the Homebrew science repository::                          | the Homebrew science repository::                           |
|                                                            |                                                             |
|   $ brew tap homebrew-science                              |   $ brew tap homebrew/science                               |
|   $                                                        |   $                                                         |
+------------------------------------------------------------+-------------------------------------------------------------+
| `Tap <https://docs.brew.sh/Taps>`_                         | `Tap <https://docs.brew.sh/Taps>`_                          |
| the IceCube Homebrew repository::                          | the Icecube Homebrew repository::                           |
|                                                            |                                                             |
|   $ brew tap icecube-spno/icecube                          |   $ brew tap icecube-spno/icecube                           |
|   $                                                        |   $                                                         |
+------------------------------------------------------------+-------------------------------------------------------------+

At this point you're now ready to install software with Homebrew. If
you're using macOS you should refer to :ref:`osx/homebrew`. Linux
users should always use Homebrew as a last resort. Your package
manager is the preferred way to get the software you need.

.. note::
   Pay attention to what Homebrew installs. It should **not**,
   unexpectedly, pull in large packages like ``python``. If it does,
   this is a **bug**, and you should stop what you're doing and let us
   know on Slack.

.. [#] MacPorts at that time was known as DarwinPorts. At some point
       there was a schism internally to that project, and DarwinPorts
       lost control of their domain. The decision was made to rename
       the project MacPorts (as that domain was available) and
       continue on. Eventually the "fake" DarwinPorts faded away, and
       MacPorts is still going strong.


.. [#] Additionally, MacPorts used its own DSL implemented as a `Tcl
       <https://en.wikipedia.org/wiki/Tcl>`_ extension. This is
       regarded within IceCube as "unpleasant", and IceCube's Tcl
       resources are in short supply.
