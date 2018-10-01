.. index:: svnintro
.. _svnintro:

Introduction to Subversion for use with IceTray
===============================================

Overview
--------

Subversion is a powerful version control system, originally designed
as a replacement for the cvs system, 
that easily handles things like meta-projects natively.

This short page is a quick summary to help people get quickly 
started using subversion (svn), and assumes some knowledge of 
revision control systems, like cvs. It covers basic concepts 
like checking out code, adding new files and projects, and 
committing your changes. More advanced options and usage are 
left for the full documentation.

Subversion is a mature, well documented system. Full documentation, 
downloads, etc are available on the 
`Subversion website. <http://subversion.tigris.org>`_

Obtaining subversion
--------------------

You'll need the subversion client software installed on your computer.

*Required*: subversion client software V1.4 or better (V1.1 or better 
will work, but will not include SVN revision/URL information 
in built libraries)

Most modern Linux distributions have Subversion included in available packages.

Otherwise, full source code and other binaries are available on 
the `Subversion website. <http://subversion.tigris.org>`_

Subversion Help
---------------

Subversion has an excellent built in help facility: ::

  > svn help


This will list all possible subcommands (commit, checkout, etc.) 
A subcommands help is found in a similar way: ::

 > svn help checkout
 > svn help commit

Additionally, the authors of Subversion have made copies of their O'Reilly 
book available `online for free. <http://svnbook.red-bean.com/>`_

Accounts/policy on code repository
-----------------------------------

Our svn server is configured to use htaccess-style web accounts. 
To request your own account, send a request to svn_admin@icecube.umd.edu
from your email address at an IceCube institution.

With this account, you'll be able to checkout code anywhere, and 
commit to any project's "trunk" (aka "head") area. Released ("tagged") 
versions of projects are only writable by a small group of people, to prevent 
them from being changed after release. 

If you only intend to check out code, you don't your own account,
as the "icecube" account 
(standard web password) is also available for checkouts only.

Organization of the repository
------------------------------

You can browse the repository with a web browser by 
`going here. <http://code.icecube.wisc.edu/svn>`_

At the top level, you'll find 3 major areas, which are 
further broken down into sub-areas:

* projects - Containing user developed projects.
   * projects/<foo>/trunk - Containing the latest (HEAD) versions of all projects.
   * projects/<foo>/releases - Containing tagged (numbered Vxx-yy-zz) releases of projects, organized by project.
   * projects/<foo>/branches - Contains other "named" branches, typically used for code testing and prototyping. 
* meta-projects - Contains the "collections of projects" workspaces used by svn to build a workspace.
* sandbox - A development area for code exchange and early prototyping work of new projects. 

Projects are specified by their URL now, for example, to reference 
the HEAD of icetray, you would use: ::

  http://code.icecube.wisc.edu/svn/projects/icetray/trunk

A tagged release of dataclasses would look like: ::

  http://code.icecube.wisc.edu/svn/projects/dataclasses/releases/V01-04-01/

When using meta-projects, you are basically checking out a 
"pre-built" workspace for you, for example: ::

  http://code.icecube.wisc.edu/svn/meta-projects/offline-software/releases/V01-06-03/

will checkout the "V01-06-03" release of offline-software, which is made of 
several projects from the releases area. They are all automatically checked 
out for you into your workspace (see below). Don't be alarmed if, when 
browsing the repository, you find that these meta-project directories 
in the repository are mostly empty. This is normal, all meta-project 
"information" is contained in special, hidden subversion property files.

Subversion commands 1: checking out, committing stuff
------------------------------------------------------

Here we'll show some of the basic subversion (svn) subcommands. Typically, 
when working in a directory that is a checked out area, you don't need to 
specify the full URL of the project. Let's take a look at a few examples. 
For those familiar with cvs, the commands are extremely similar.

Checking out code 
^^^^^^^^^^^^^^^^^^
::

  > svn checkout http://code.icecube.wisc.edu/svn/projects/linefit/trunk

will checkout the HEAD of the linefit project into a "trunk" 
subdirectory of your current workspace. Some useful alternatives: ::

  > svn checkout http://code.icecube.wisc.edu/svn/projects/linefit/trunk mywork

will put the checkout in a subdirectory called "mywork". ::

  > svn checkout http://code.icecube.wisc.edu/svn/projects/linefit/trunk --username icecube

will use the icecube account on the subversion server. 

Remember, the "icecube" account and password can only be used to 
checkout code, not for imports or commits!

Checking out meta-projects 
^^^^^^^^^^^^^^^^^^^^^^^^^^
::

  > svn checkout http://code.icecube.wisc.edu/svn/meta-projects/offline-software/releases/V01-09-07 workspace

will create a subdirectory "workspace" and populate it with the set of 
projects that makes up the V01-09-07 tagged release of offline software. 
This directory is a fully put together, ready to compile workspace. 

Seeing the contents of a meta-project 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once you've checked out a meta-project, you can view the "svn:externals" file. 
This magic file contains the actual locations in SVN of the projects that 
make up the meta-projects, including the release version numbers. To see 
the contents of the meta-project, in the top level of your workspace run: ::

  > svn propget svn:externals .

And you'll see an ouput like: ::

  dataclasses http://code.icecube.wisc.edu/svn/projects/dataclasses/releases/V01-05-00
  dataio http://code.icecube.wisc.edu/svn/projects/dataio/releases/V01-09-00
  ...

Adding and committing code to an existing project 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suppose you've checkout a project using a command like above, for example: ::

  > svn checkout http://code.icecube.wisc.edu/svn/projects/linefit/trunk mywork

And now, in the "mywork" subdirectory, you've added a new file mytestfile.cxx 
and you need to add it to the svn repository: ::

  > svn add mytestfile.cxx

now that your file is added, you must commit it: ::

  > svn commit mytestfile.cxx

An editor window will appear. Please enter a short, but descriptive 
summary of the changes and additions you've made. If you edit many files
in the mywork project, you can commit all changed files in your working area. 
From within: ~/mywork: ::

 > svn commit file1 file2 file3

Again, an editor window will appear. Please enter a short, but descriptive 
summary of the changes and additions you've made.  It is best to explicitly list
the files you are commiting to ensure no unplanned changes are commited.

If you would like to use a different editor to enter these commit logs, 
then set the SVN_EDITOR environment variable to the full path of your your 
favorite editor. For example (bash): ::

  > export SVN_EDITOR=/usr/bin/vim

or (csh, tcsh): ::

  > setenv SVN_EDITOR /usr/bin/vim

Subversion commands 2: releases etcetera
----------------------------------------

Adding a new project to the repository 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suppose you've developed a new module for icetray, and you are ready 
to commit to the the "sandbox" area. Your module is called "super_fly_fitter", 
and is in a directory of the same name in your workspace. Then: ::

  > svn import ./super_fly_fitter http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter

Will add and commit it to the "sandbox" repository. Remember to add a 
descriptive commit message. Keep in mind that the copy on your local 
disk that you imported from is not under svn control, so if you wish 
to continue working on your module, you'll need to check out an 
"svn-controlled" copy. See above.

Making a trunk, branches, releases for your existing project 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When your super_fly_fitter turns out to be a success and other users depend 
on it, or it gets included in metaprojects, it's time to do more structured 
version management. Make sure that all relevant changes in your local copy 
are committed, then do: ::

  > svn mv http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter http://code.icecube.wisc.edu/svn/sandbox/tmp_super_fly_fitter
  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter
  > svn mv http://code.icecube.wisc.edu/svn/sandbox/tmp_super_fly_fitter http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk
  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches
  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/releases

Once this project is ready for wider release, a release manager
can move this whole directory tree from the sandbox to the projects area.

Adding a new project, directly with a trunk, branches, releases 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Making tmp directories in the SVN archive is not so nice, so if you have you 
know at the start that your module might be a serious, then it's best to 
anticipate releases, branches etc. during the initial commit to the 
repository. So instead of the commands in the first bullet of this list 
you'd do: ::

  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter
  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches
  > svn mkdir http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/releases
  > svn import ./super_fly_fitter http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk

Making a new branch 
^^^^^^^^^^^^^^^^^^^

There are various situations where you'd like to split the trunk of development 
into one or more parallel development lines. For instance if you'd like to 
experiment with some particularly invasive new code. Or, on the contrary, 
if you'd like to work towards a release. Then you'd do something like: ::

  > svn cp http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches/wild_new_dangerous_Oct2007

or: ::

  > svn cp http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches/V01-42-00.rc1


Making a release of your project 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Easy. If you are ready for release of version 1.2.3, then do: ::

  > svn cp http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/releases/V01-02-03

A release is a release, you should not edit it anymore afterwards. You can 
cheat a little bit (a subversion directory cannot be made "readonly", unfortunately), 
but after you have actually announced the release, it should remain frozen. 
If you made a mistake, then fix it in the trunk and make a fast new incremental 
release. If your release is supposed to be part of some metaproject release, 
and the versions of some other projects, on which your super_fly_fitter 
depends, are in there with a slightly different version than you have 
been working with, then you may want to first split the trunk of your 
project off to a branch, e.g.: ::

  > svn cp http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/trunk http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches/V01-02-03-rc1

so that you (and your colleagues) can test it in the new environment, make fixes and then copy or move it to a real release: ::

  > svn mv http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/branches/V01-02-03-rc1 http://code.icecube.wisc.edu/svn/sandbox/super_fly_fitter/releases/V01-02-03


Adding a new metaproject to the repository 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is exactly like adding a new project, except that you'll also 
fill the svn:externals, which is basically a list of other projects 
which you would like to include within the metaproject. Usually a 
metaproject has very little own code, and is mostly defined 
by its included projects.

A common way to start a new metaproject, say super_fly_business, is to base 
it on a release some other metaproject, for example 
icerec version V01-05-01. You can do that by first reading the 
externals list of that other metaproject, like: ::

  > svn propget svn:externals http://code.icecube.wisc.edu/svn/meta-projects/icerec/releases/V01-05-01 > othermeta.list

Then you edit that othermeta.list file, to omit/add projects and change 
versions. (Of course you could also write that list from scratch.) Each 
line in this file specifies a project. Here you see three example lines with: 
* project which should be included with a fixed release, 
* a project for which you always want to see the current trunk
* a project you want to use the state of its trunk at a specific revision number: ::

  dataclasses http://code.icecube.wisc.edu/svn/projects/dataclasses/releases/B01-10-01
  directwalk http://code.icecube.wisc.edu/svn/projects/directwalk/trunk
  rime -r 22652 http://code.icecube.wisc.edu/svn/projects/rime/trunk (!)

**CAUTION** should be used when using a specific revision number.  
'svn update' will take you literally and enforce this, sometimes at the 
expense of local mods.  You are better off making
a release from this specific revision (or a branch), and including that.

Once you (re)defined the list of projects and their versions, you do:

> svn propset svn:externals -F othermeta.list http://code.icecube.wisc.edu/svn/meta-projects/super_fly_business/trunk

Alternatively, if the top directory of your metaproject is your current working 
directory of your shell session: ::

 > svn propset svn:externals -F othermeta.list .
 > svn commit -N

The -N is there so that svn does not dig into your project directories 
looking for changes to make. Without the -N, it will also commit any code u
pdates you have made. Setting the properties with these commands will only 
have effect on the projects included in your local copy of the metaproject 
after you do ::

  > svn update

This will actually check out the projects (if that did not happen already) 
and/or up/down-grade them to the requested versions in accordance with the 
svn:externals list. Note that if you remove a project from the svn:externals, 
then the svn update will not remove it from your local copy, you need to do 
that manually.

Making a release of a metaproject 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The same as for a project. If your metaproject contains several 
project which are under lively development, then you might want to 
first split the trunk off to a branch and let that freeze out, 
before actually doing the release.


Subversion tricks, tips, etc.
-----------------------------

* Use the $SVN environment variable. This way you don't have to type
  the full repository path with each checkout, commit, copy,
  whatever::

  > export SVN=http://code.icecube.wisc.edu/svn

* SVN diff can't ignore whitespace. But you can override the "diff"
  command and use the system diff::

  > svn diff --diff-cmd diff -x -uw <path1> <path2>

* To ignore a file or a group of files in a directory, use "propedit"::

   > svn propedit svn:ignore .  
   (add *.pyc to the list)

You'll need to commit this change once you're done editing. 
