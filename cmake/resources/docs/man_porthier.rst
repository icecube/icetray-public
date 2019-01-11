porthier - layout of the ports-filesystems (manpage)
====================================================

DESCRIPTION
-----------
A sketch of the filesystem hierarchy as it is applied on ports being dis-
tributed by DarwinPorts::

  /${prefix}/     The base of the DarwinPorts filesystem hierarchy. By
		  default set to "/opt/local/"

		  bin/      common utilities, programming tools, and appli-
			    cations

		  etc/      system configuration files and scripts

		  games/    useful and semi-frivolous programs

		  include/  standard C include files

		  lib/      archive libraries

		  libexec/  system daemons & system utilities (executed by
			    other programs)

		  sbin/     system programs and administration utilities

		  share/    architecture-independent files

			    doc/       miscellaneous documentation
			    examples/  various examples for users and pro-
				       grammers
			    info/      GNU Info hypertext system
			    java/      applications based on Java
			    locale/    localization files
			    man/       manual pages
			    misc/      misc system-wide ASCII text files

		  var/      multi-purpose log, temporary, transient, and
			    spool files

			    db/     misc. automatically generated system-
				    specific database files

				    dports/  DarwinPorts runtime data

			    spool/  directory containing output spool files
			    log/    misc. system log files
			    run/    system information files describing
				    various info about system since it was
				    booted

  /Applications/DarwinPorts/
		  Native Mac OS X applications

  /Library/Frameworks/
		  Native Mac OS X Library Frameworks

AUTHORS
-------
     Felix Kronlage <fkr@opendarwin.org>

BSD                            October 11, 2003                            BSD
