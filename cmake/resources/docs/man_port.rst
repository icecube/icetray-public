port manpage
============

NAME
----
     port - build darwin ports

SYNOPSIS
--------
     port [-vdqfonausbck] [-D portdir] target [portname] [options ...]
          [variants ...]

     port provides file

DESCRIPTION
-----------

     port is designed to read and execute the build instructions of a darwin
     port. If no portdir or portname is specified, the current working direc-
     tory is assumed.  If no target is specified, the usage is displayed.
     Port options are specified as key=value pairs and take precedence over
     individual port options and system-wide settings.

     Port variants are specified as +name, which indicates the variant is
     desired, and -name, which indicates the variant is not desired. In case
     of ambiguities, a port can be fully specified with the format
     version_revision+variants

     Installed ports can be activated or deactivated without being unin-
     stalled. A port can be installed if another version/variant is installed
     but deactivated.

     The provides command determines which port owns a given file.

OPTIONS
-------
     -v       verbose mode (generate verbose messages)

     -d       debug mode (generate debugging messages, implies -v)

     -q       quiet mode (suppress messages)

     -c       autoclean mode (execute clean after install)

     -k       keep mode (don't autoclean after install)

     -n       dont follow dependencies in upgrade (only for upgrading)

     -a       upgrade all installed ports (only for upgrading)

     -u       uninstall non-active ports when upgrading and uninstalling

     -D       specify portdir

     -f       force mode (ignore state file)

     -o       honor state files older than Portfile

     -s       source-only mode (only build and install from source, ignores
              all binary archives, does not create/recreate binary archives)
              (only applies when archive mode is enabled)

     -b       binary-only mode (only build and install from binary archives,
              ignores source, fails when no archive present, and does
              not create/recreate binary archives from source) (only
              applies when archive mode is enabled)

USER TARGETS
------------

port knows several targets. the ones commonly used by users of darwin-
ports are:

install
  install and activate portname.

uninstall
  deactivate and uninstall portname.  to uninstall all installed but inactive ports, specify -u.  for example::

    port uninstall vim
    port -u uninstall

activate
  activate the installed portname.

deactivate
  deactivate the installed portname.

installed
  list all installed ports.

outdated
  list the installed ports that need upgrading.

upgrade
  upgrade the installed portname.  to upgrade all ports, specify -a.  for
  example::

	port upgrade vim
	port -a upgrade

  to upgrade portname wihtout following its dependencies, specify -n.  for
  example::

	port -n upgrade ethereal

selfupdate
  updates the darwinports system, both the dports tree and if needed the
  base tools, installing the newest infrastructure if available. to update
  you would typically do::

	sudo port -d selfupdate

sync
  performs a sync operation only on the dports tree of a darwinports
  installation, pulling in the latest revision available of the portfiles.
  to update you would do::

	sudo port -d sync

clean
  clean the files used for building portname.  to just remove the work
  files, specify --work.  to remove the distribution files (tarballs, etc),
  specify --dist.  to remove the archive(s) for the current version of a
  port, specify --archive.  to remove the work files, distribution files
  and archives, specify --all.  for example::

	port clean --dist vim
	port clean --archive vim

  to remove only certain version(s) of a port's archives ( version is any
  valid unix glob pattern), you can use::

	port clean --archive vim 6.2.114

  or::

	port clean --archive vim '6.*'

list
  list all available ports.

search
  search for an available port whose name matches a regular expression.

	port search vim

info
  displays the meta-information about an available portname.

variants
  lists the build variants available for portname.

deps
  lists the other ports that are required for building and running
  portname.

contents
  lists the files installed by portname.

version
     Display the release number of the installed DarwinPorts infrastructure.

DEVELOPER TARGETS
-----------------

The targets that are often used by Port developers are intended to
provide access to the different phases of a Port's build process:

unarchive
  Unpack the port from a pre-built binary archive. When archive mode is
  enabled, this command is called automatically, prior to fetch, to check
  for an existing binary archive to unpack. If found, it is unpacked and
  all stages up to install are then skipped.

fetch
  Fetches the distribution files required to build portname.

extract
  Extracts the distribution files for portname.

patch
  Applies any required patches to portname's extracted distribution files.

configure
  Runs any configure process for portname.

build
  Build portname.

destroot
  Installs portname to a temporary directory.

test
  Tests portname.

archive
  Archive the port for a later unarchive.  When archive mode is enabled,
  binary archives will be created automatically whenever an install is per-
  formed, or when the archive target is called explicitly.

PACKAGING TARGETS
-----------------
There are also targets for producing installable packages of ports:

pkg
  Creates an OS X installer package of portname.

mpkg
  Creates an OS X installer metapackage of portname and its dependencies.

dmg
  Creates an internet-enabled disk image containing an OS X package of
  portname.

rpmpackage
  Creates an RPM package of portname.

EXAMPLE
-------

The following demonstrates invoking port with the "extract" target on
portdir "textproc/figlet" and extract.suffix set to ".tgz"::

      port extract -D textproc/figlet extract.suffix=.tgz

DIAGNOSTICS
-----------
     The port utility exits 0 on success, and >0 if an error occurs.


AUTHORS
-------
     Landon Fuller <landonf@opendarwin.org>
     Kevin Van Vechten <kevin@opendarwin.org>
     Jordan K. Hubbard <jkh@opendarwin.org>
     Ole Guldberg Jensen <olegb@opendarwin.org>
     Robert Shaw <rshaw@opendarwin.org>
     Chris Ridd <cjr@opendarwin.org>


BSD August 24, 2002
