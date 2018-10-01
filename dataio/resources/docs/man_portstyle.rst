portstyle - style guide for ports their associated files (manpage)
==================================================================

DESCRIPTION
-----------

A port consists of a directory and its contents, within a category
subdirectory of the root of the ports tree.  This document serves as a
reference for the directory structure of a single port and the layout
of the files within.

The only required file in a port is Portfile.  The port file should be
thought of as a set of declarations rather than a piece of code.  It is
best to format the port file is if it were a table consisting of keys and
values.  In fact, the simplest of ports will only contain a small block
of values.  Therefore, whitespace should not be strown about haphazardly.
Nicely formatted compact tables will result in more values being visible
at the same time.

At the top of the port file is the requirement statement.  There is a
single space between the ``PortSystem`` statement and the version number.

Use tabs to align items into columns.  The left side should consist of
single words, and will be separated from the more complex right side
by a number of tabs that will result in all items begining on the same
column.  Variable assignments such as ``set libver`` can be considered
a single word on the left side, with a single space between ``set``
and the variable name, followed by tabs to line up with the right
side.

The first category listed for a port is the port's primary category
and should coincide with the subdirectory that the port resides in.
Additional categories can be listed, but should be descriptive and
accurate.

Obviously a simple table of values won't do for all ports.  More
complicated ports will need to have things like multiple distribution
files, custom targets, and variants.  When items require multiple
lines, they can be separated from the previous and next items with a
blank line.  Indent the additional lines to the same column that the
right side begins on in the first line.  Should the items require
``{}`` braces, the braces may appear on the first and last lines
rather than on their own lines.  This is done because the right side
of the port file is already indented, and to make port file read like
a table.  Use a single space after the brace.  Indent additional lines
to this same column.  If a single state- ment needs to span multiple
lines, use a 2 space indentation from the first line for all
additional lines.

Variants are like any other part of the port file.  Space, indent, and
format them the same way other sections are formatted.  The actual
variant statement, ``variant`` followed by the name of the variant,
can be treated as a single statement and thus placed together on the
left side and separated by a single space.

Sometimes it is useful to have a block of raw code for a port file.  In
these cases, either place the code at the bottom of the port file or pull
it in from an external file, and simply format it in a well-structured
manner with sensible indentation like any other code should be.

Patch files reside in the files/ directory in the port's base
directory.  There should be one patch file for every file that needs
to be patched.  It is perfectly reasonable to use provided patches
that affect multiple files, but do not create new patches that do so.
Their names should begin with ``patch-``.  The rest of the name should
come from the name of the file that they apply to.  In many cases just
the base name of the file being patched is enough.  For files such as
``Makefile`` this may not be the case.  In this case, use enough
components of the file's path to uniquely distinguish the file.
Separate the components with '-' characters.  The patch should apply
with ``patch -p0`` from the working source directory of the port.

Try to adhere to these guidelines, but remember that this is only a
guide.  No set of rules can adequately cover all cases, and in some cases
the best solution is to break the rules.

AUTHORS
     Eric Melville <eric@opendarwin.org>

Darwin                          March 11, 2003                          Darwin
