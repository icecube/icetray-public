#!/usr/bin/env perl
#
#  $Id$
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#  
#  install_shlib.pl:
#
#  How stunningly irritating it has turned out to be to do this
#  portably.  We take the library that the makefiles discover and walk
#  through the directory that it originates in, copying what that file
#  points to, if anything, and recreating any other links that point to
#  this library as well, since ld might decide to use, say liba.so.2
#  instead of liba.so...  and liba.so and liba.so.2 might both be
#  symlinks to liba.so.2.0.0

print "***\n*** DEPRECATED, use install_shlib.py instead\n***\n"
exit(1)


use File::Copy;
use File::Basename;

$#ARGV == 1 || die "Usage: $0 source_file destination_directory";

$src = $ARGV[0];
$dst = $ARGV[1];

$srcdir    = dirname($src);
$srcfile   = basename($src);

if (-l $src)
{

    $linkedto = readlink $src;
    unlink($dst . "/" . $linkedto);
    system($0,  $srcdir . "/" . $linkedto, $dst);
#    copy($srcdir . "/" . $linkedto, $dst) || die "Couldn't copy $directory/$linkedto to $dst: $!";
    chdir($dst) || die "Couldnt chdir to $dst: $!";
    unlink($srcfile);
    symlink($linkedto, $srcfile) || die "Couldnt symlink $linkedto to $srcfile in directory $dst: $!";
    $mode = (stat($srcdir . "/" . $linkedto))[2] & 00777;
#    printf "Setting permissions %04o\n", $mode;
    chmod $mode, $linkedto || die "couldn't set perms on $linkedto: $!";

    opendir SRCDIR, $srcdir;
    @files = readdir(SRCDIR);
    foreach $file (@files) 
    {
	if (-l "$srcdir/$file" && (! -l "$dst/$file"))
	{
	    $thislink = readlink "$srcdir/$file";
	    if ($thislink eq $linkedto) 
	    {
#		print "  Linking $srcdir/$file ($thislink) -> $linkedto\n";
		symlink($linkedto, $file);
	    }
	}
    }

} else {

    unlink($dst . "/" . $srcfile);
    copy($src, $dst) || die "Couldn't copy $src to $dst: $!";
}

