#!/usr/bin/env perl

# 
#  This is the same transformation found in I3.h, the main header
#  for icecube as a whole.  gcc 4.3.2 becomes 40302.  Makes it 
#  possible to do string/numeric compares. 
#

$str = <>;
($maj, $min, $patch) = ($str =~ /(\d+)\.(\d+)\.(\d+)/);
$version = $maj * 10000 + $min * 100 + $patch;
printf "%d", $version;




