#!/bin/bash -e

h5ls --help >/dev/null 2>/dev/null || exit 0

book() {
    hdfwriter-book ${I3_TESTDATA}/sim/Level3_nugen_numu_IC86.2012.011069.000000_20events.i3.bz2 $@ -n 2 --streams Final
}

fail() {
    echo $@ 2>&2
    exit 1
}

book a.hdf5 --keys BestTrack
[ $(h5ls a.hdf5 | grep BestTrack | wc -l) -gt 0 ] || fail "BestTrack not found in a.hdf5"

book b.hdf5 --keys MuEXAngular4
[ $(h5ls b.hdf5 | grep MuEXAngular4 | wc -l) -gt 0 ] || fail "MuEXAngular4 not found in b.hdf5"

rm c.hdf5 2>/dev/null || true
hdfwriter-mix a.hdf5 b.hdf5 -o c.hdf5

[ $(h5ls c.hdf5 | grep BestTrack | wc -l) -gt 0 ] || fail "BestTrack not found in c.hdf5"
[ $(h5ls c.hdf5 | grep MuEXAngular4 | wc -l) -gt 0 ] || fail "MuEXAngular4 not found in c.hdf5"
