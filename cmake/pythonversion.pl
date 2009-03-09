#!/usr/bin/env perl

# 
#  This is the same transformation found in gccversion.pl.
#  "Python 2.3.4" becomes "20304".  Makes it possible to do 
#  string/numeric compares. 
#

$str = $ARGV[0];
chomp($str);

($maj, $min, $patch) = ($str =~ /(\d+)\.(\d+)\.(\d+)/);
$version = $maj * 10000 + $min * 100 + $patch;
printf "%d", $version;
