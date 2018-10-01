Portfile - Darwinports description file reference (manpage)
===========================================================

DESCRIPTION
-----------
     A complete reference of all available Portfile variables and example syn-
     tax.  Portfiles consist of valid TCL, allowing for simple key/value pair
     syntax as well as utilization of TCL's extensive functionality.
     Portfiles are encoded in UTF-8.

     The Darwinports System uses a target dependency system based on a
     depends/provides model, allowing for targets to be registered and exe-
     cuted in the correct order based on their individual requirements.

     A Portfile author needs to be aware of the various standard targets, the
     options that they require and the variables that both the targets and the
     port system provide.

MAIN VARIABLES
--------------
     All ports are required to set certain variables.

     name
         Full name of port.
         Type: required
         Example::

               name XFree86

     version
         Upstream version of software.
         Type: required
         Example::

               version 4.2.1

     epoch
         If a port's version numbering changes such that a newer version looks
         older than the previous version, the epoch should be increased. Often
         the epoch is formatted like a date.
         Type: optional
         Default: 0
         Example::

               epoch 20041231

     description
         One line description of the software and what it does.
         Type: required
         Example::

               description Dictionary Server Protocol (RFC2229) client

     long_description
         A verbose description of the software and what it does.
         Type: required
         Example::

               long_description The GNU Image Manipulation Program \
                   (GIMP) is a powerful tool for the preparation and \
                   manipulation of digital images. The GIMP provides \
                   the user with a wide variety of image manipulation, \
                   painting, processing, and rendering tools.

     revision
         Local revision number of Portfile.  Increment for port revisions
         which would change its installation in any way.
         Type: optional
         Default: 0
         Example::

               revision 1

     categories
         Categories to which this port belongs.
         Type: required
         Example::

               categories spelling textproc

     maintainers
         E-mail address(es) of port maintainer(s).
         Type: required
         Example::

               maintainers landonf@opendarwin.org kevin@opendarwin.org

     platforms
         Declares which platforms are supported by the port.
         Type: required
         Values: darwin freebsd
         Example::

               platforms darwin

     worksrcdir
         Path to source directory relative to workpath.
         Type: optional
         Default: ${distname}
         Example::

               worksrcdir ${distname}-src-${version}

     distname
         Name of distribution file, without the extract.suffix.
         Type: optional
         Default: ${name}-${version}
         Example::

               distname ${name}-${version}-src

     master_sites
         List of sites to fetch distfiles from or a predefined mirror site
         list. If set to a predefined mirror site, without a subdirectory
         being defined, the portname is used as the name of the subdirectory.
         Type: required
         Example::

               master_sites ftp://ftp.cdrom.com/pub/magic \
                   sourceforge

     checksums filename type checksum [filename type checksum ...]
         List of checksums for the distfiles.  The checksum type can currently
         be md5 or sha1. The filename can be omitted if there is only one dis-
         tfile.
         Type: required
         Example::

               checksums dictd-1.7.1.tar.gz md5 81317b86ea0a5df0163900ad2e6bb12c \
                       magic-words-1.7.1.tar.gz md5 897a005182928613eadd30c267ce9c5b

         Example (ledit 1.11):

               checksums md5 a2d38ba641682509c1e964ad699a9dd2 \
                       sha1 1fb6443b5fdf3c83787953f06282d256477c1288

     use_automake
         If set to yes, run the automake target to build any Makefile.in files
         for use by configure.
         Type: optional
         Default: no
         Example::

               use_automake yes

     use_autoconf
         If set to yes, run the autoconf target to build any configure script
         required.
         Type: optional
         Default: no
         Example::

               use_autoconf yes

     use_configure
         If set to yes, run the configure target to configure the build.
         Type: optional
         Default: yes
         Example::

               use_configure no

TARGET HOOKS
------------
     A number of hooks are available for customizing many of the standard tar-
     gets that port(1) executes. The targets supporting these hooks are fetch,
     automake, autoconf, configure, build, destroot, and test.  The hooks are:

     target.dir
         Directory in which to run the target.
         Example::

               automake.dir src

     target.env
         Change the environment the target is run in. This is often overridden
         on a per Portfile basis.
         Example::

               configure.env CFLAGS=-O

     target.pre_args
         Additional arguments passed before the main arguments.
         Example::

               extract.pre_args -cd

     target.args
         Main arguments to pass to the target.  This is often overridden on a
         per Portfile basis.
         Example::

               configure.args --enable-fooble

     target.post_args
         Additional arguments passed after the main arguments.
         Example::

               extract.post_args | tar xf -

RUNTIME VARIABLES
-----------------
     Read-only access to the Darwinports configuration is provided.

     prefix
         Install prefix
         Type: optional
         Default: /opt/local

     libpath
         Location of ports-specific TCL libraries.
         Type: read-only

     workdir
         Path to work directory relative to portpath.  Where possible use
         workpath instead.
         Type: read-only
         Default: work

     workpath
         Full path to work directory.
         Type: read-only
         Default: ${portpath}/${workdir}

     worksrcpath
         Full path to working sources (where port has unpacked itself).
         Type: read-only
         Default: ${workpath}/${worksrcdir}

     filesdir
         Path to port files relative to portpath.
         Type: read-only
         Default: files

     distpath
         Location to store downloaded distfiles.
         Type: read-only
         Default: ${sysportpath}/distfiles/${portname}/

     os.arch
         Identifies hardware type (eg "powerpc").
         Type: read-only

     os.version
         Version number of operating system (eg "7.0").
         Type: read-only

     os.endian
         Endianness of the processor (eg "bigEndian").
         Type: read-only

     os.platform
         Operating system name (eg "darwin").
         Type: read-only

     x11prefix
         Absolute path to X11 (eg /usr/X11R6)
         Type: read-only

DEPENDENCY OPTIONS
------------------
     Each dependency for the port is expressed in the format:

     type:<filename>:<port>

     Where type is "bin" if <filename> is a program, "lib" if it is a library,
     or "path" if it is a path to an installed file. If the dependency is
     really on a whole port and not an individual file, then the alternative
     syntax:

     port:<port>

     should be used.  <port> represents the name of an existing Darwinports
     port.

     depends_build
         List of dependencies to check before build, destroot, install, and
         package targets.
         Type: optional
         Example::

               depends_build bin:autoconf:autoconf

     depends_run
         List of dependencies to check before destroot, install and package
         targets.
         Type: optional
         Example::

               depends_run bin:bash:bash

     depends_lib
         List of dependencies to check before configure, build, destroot,
         install, and package targets.
         Type: optional
         Example::

               depends_lib lib:libfetch.3:libfetch

FETCH OPTIONS
-------------
     Fetch all distribution files and patches.

     master_sites.mirror_subdir
         Subdirectory to append to all mirror sites for any list specified in
         master_sites.
         Type: optional
         Default: ${name}
         Example::

               master_sites.mirror_subdir magic

     patch_sites
         List of sites to fetch patchfiles from or a predefined mirror site
         list.
         Type: optional
         Default: ${master_sites}
         Example::

               patch_sites ftp://ftp.patchcityrepo.com/pub/magic/patches

     patch_sites.mirror_subdir
         Subdirectory to append to all mirror sites for any list specified in
         patch_sites.
         Type: optional
         Default: ${name}
         Example::

               patch_sites.mirror_subdir magic

     extract.suffix
         Suffix to append to distname.
         Type: optional
         Default: .tar.gz
         Example::

               extract.suffix .tgz

     distfiles
         List of distribution files to fetch from master_sites.
         Type: optional
         Default: [suffix ${distname}]
         Example::

               distfiles magicsource.tar.gz cluebat.tar.bz2

     patchfiles
         List of patches to fetch and apply.
         Type: optional
         Example::

               patchfiles japanese-widechar-fix.diff
               japanese-localization.diff

     use_zip
         Use zip.
         Sets extract.suffix to: .zip
         Sets extract.cmd to: unzip
         Sets extract.pre_args to: -q
         Sets extract.post_args to: "-d $portpath/$workdir"
         Type: optional
         Example::

               use_zip yes

     use_bzip2
         Use bzip2.
         Sets extract.suffix to: .bzip2
         Sets extract.cmd to: bzip2
         Type: optional
         Example::

               use_bzip2 yes

     dist_subdir
         Create a sub-directory in distpath to store all fetched files.
         Type: optional
         Default: ${name}
         Example::

               dist_subdir vim${version}

   FETCHING FROM CVS
     As an alternative to fetching distribution files, pulling the sources
     from a CVS repository is supported. Use of CVS can give rise to non-
     reproducible builds, so it is strongly discouraged.

     cvs.tag
         CVS tag identifying the code to checkout.
         Type: optional

     cvs.module
         CVS module to check the code out from.
         Type: optional

EXTRACT OPTIONS
---------------
     Extract all compressed/archived files.

     extract.only
         List of files to extract into workpath.
         Type: optional
         Default: ${distfiles}
         Example::

               extract.only worksrc-1.4.4.tar.gz

     extract.cmd
         Command to perform the extraction.
         Type: optional
         Default: gzip
         Example::

               extract.cmd bzip2

BUILD OPTIONS
-------------
     Execute necessary build commands.

     build.cmd
         Make command to run relative to worksrcdir.
         Type: optional
         Default: make
         Example::

               build.cmd pbxbuild

     build.type
         Defines which 'make' is required, either 'gnu' or 'bsd'.  Sets
         build.cmd to either gnumake or bsdmake accordingly.
         Type: optional
         Default: gnu
         Example::

               build.type bsd

     build.target
         Target passed to build.cmd.
         Type: optional
         Default: all
         Example::

               build.target all-src

DESTROOT OPTIONS
----------------
     Execute necessary commands to install into a temporary destination root
     ("destroot") staging area.

     destroot.cmd
         Install command to run relative to worksrcdir.
         Type: optional
         Default: ${build.cmd}
         Example::

               destroot.cmd pbxbuild

     destroot.type
         Defines which 'make' is required, either 'gnu' or 'bsd'.  Sets
         destroot.cmd to either gnumake or bsdmake accordingly.
         Type: optional
         Default: ${build.type}
         Example::

               destroot.type gnu

     destroot.destdir
         Arguments passed to destroot.cmd in order to install correctly into
         the destroot.
         Type: optional
         Default: DESTDIR=${destroot}
         Example::

               destroot.destdir prefix=${destroot}${prefix}

     destroot.target
         Install target to pass to destroot.cmd.
         Type: optional
         Default: install
         Example::

               destroot.target install-src

     destroot.umask
         Umask to use during destroot.
         Type: optional
         Default: 022
         Example::

               destroot.umask 002

     destroot.keepdirs
         List of directories that should not be pruned if empty upon destroot
         completion.
         Type: optional
         Example::

               destroot.keepdirs ${destroot}${prefix}/var/log/mysql

TEST OPTIONS
------------
     Execute commands to run test suites bundled with a port.

     test.run
         Enable running test suites bundled with a port.
         Type: optional
         Example::

               test.run yes

     test.cmd
         Test command to run relative to worksrcdir.
         Type: optional
         Default: ${build.cmd}
         Example::

               test.cmd checks.sh

     test.target
         Test target to pass to test.cmd.
         Type: optional
         Default: test
         Example::

               test.target checks

STARTUPITEMS OPTIONS
--------------------
     If a port needs to run on system startup, it can use the Darwinports
     startup mechanism.

     startupitem.name
         Displayed name of the startup item.
         Type: required
         Example::

               startupitem.name OpenSSH

     startupitem.type
         Form of startup item to install.
         Type: optional
         Default: SystemStarter
         Values: SystemStarter RCng

VARIANT OPTIONS
---------------
     Darwinports allows for conditional modification to be specified in a
     Portfile, allowing for user-customization of a software's build-time set-
     tings.

     variant [requires variant] [conflicts variant]
         The value is usually a TCL script which modifies other port's
         Portfile variables. Dependencies and conflicts with other variants in
         the same port can be expressed with requires and conflicts.  The
         example adds a "gnome" variant to a port.
         Type: optional
         Example::

               variant gnome requires glib { configure.args-append --with-gnome \
                   depends_lib-append lib:gnome-session:gnome-session }

     default_variants
         If variants are defined, then the default_variants value lists which
         variants are enabled by default.
         Type: optional
         Example::

               default_variants +ssl +tcpd

PLATFORM OPTIONS
----------------
     Darwinports allows for platform-specific conditional modification to be
     specified in a Portfile, much like variants, for handling differences
     between platforms and versions of the same platform.

     platform platform [version] [arch]
         The platform key is used to begin the darwin platform definitions as
         shown in the examples from the databases/db4 and devel/libidl1
         Portfiles respectively.
         Type: optional
         Example::

               platform darwin 6 { configure.args-append   --enable-tcl \
                       --with-tcl=/System/Library/Tcl/8.3 }

         Example::

               platform darwin powerpc { configure.args-append \
                       --host=${os.arch}-apple-rhapsody${os.version} }
               platform darwin i386 { configure.args-append \
                       --host=i386-gnu-rhapsody${os.version} }

TCL EXTENSIONS
--------------

     A number of TCL extensions are available for use in Portfiles.

     xinstall [-B suffix] [-b] [-C] [-f flags] [-g group] [-M] [-m mode] [-o
         owner] [-p] [-S] [-s] [-W dir] [file ...] destination

     xinstall -d [-B suffix] [-b] [-C] [-f flags] [-g group] [-M] [-m mode]
         [-o owner] [-p] [-S] [-s] [-W dir] directory
         Install file(s) to a target file or directory. The options are
         intended to be compatible with install(1):

         -b      Backup any existing files with an .old extension.

         -B      Specify a different backup suffix for the -b flag.

         -C      Only copy a file if it is different.

         -d      Create directories, including (if necessary) parent directo-
                 ries.

         -f      Specify target flags, see chflags(1) for details.

         -g      Specify the group.

         -M      Disable use of mmap(2).

         -m      Specify an alternate mode. The default is 0755. See chmod(1)
                 for defails.

         -p      Preserve the modification time.

         -S      Copy safely, using a temporary file.

         -s      Strip binaries using strip(1).

         -W      Change to dir before working.

     adduser username [uid=uid] [gid=gid] [passwd=passwd] [realname=realname]
         [home=home] [shell=shell]
         Add a new local user to the system with the specified uid, gid, pass-
         word, real name, home directory and login shell.

     existsuser username
         Check if a local user exists.

     nextuid
         Returns the highest used uid plus one.

     addgroup group [gid=gid] [passwd=passwd] [users=users]
         Add a new local group to the system, with the specified gid, password
         and with a list users as members.

     existsgroup group
         Check if a local group exists.

     nextgid
         Returns the highest used gid plus one.

     reinplace regex filename
         Provide in-place sed like editing of a file.
         Example::

               reinplace "s|/usr/local|${prefix}|g" doc/manpage.1

     file
         Standard TCL command to manipulate file names and attributes. See
         file(n).

     system commandline
         Execute a program. See system(3).  For calls to install(1) please use
         xinstall.  For calls to mv(1), cp(1), rm(1) or similar, please use
         file.

     variant_isset variant
         Checks if the given variant is being built.

     variant_set variant
         Set the given variant.

     variant_unset variant
         Unset the given variant.

     variable-append item
         Append item to the variable.
         Example::

               configure.args-append --with-gnomedb

     variable-delete item
         Delete item from the variable.
         Example::

               configure.args-delete --with-gnomedb

     readdir directory
         Return the list of elements in a directory, excluding . and ...

     strsed string pattern
         Perform ed(1)/tr(1)-like search, replace, and transliteration on a
         string.

     mktemp template
         Create a temporary file using a template.  See mktemp(3).

     mkstemp template
         Create a temporary file securely using a template.  See mkstemp(3).

     md5 file ...
         Compute the MD5 hashes of the file(s).

     rpm-vercomp versionA versionB
         Compare two RPM-format versions for equality.

     sudo password command ...
         Execute command using sudo with the provided password.

     ui_debug message
     ui_error message
     ui_info message
     ui_msg message
     ui_warn message

         Display a message to the user, at various different levels.
         Example::

               ui_msg "Add each user to the system using the clamav command"

AUTHORS
-------
     Landon Fuller <landonf@opendarwin.org>
     Kevin Van Vechten <kevin@opendarwin.org>
     Jordan K. Hubbard <jkh@opendarwin.org>
     Chris Ridd <cjr@opendarwin.org>

BSD                             August 17, 2003                            BSD
