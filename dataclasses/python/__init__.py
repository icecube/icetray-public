from icecube.load_pybindings import load_pybindings
import icecube.icetray # be nice and pull in our dependencies

# XXX Work around a stupid bug with certain combinations of Boost, compiler, and
# RTLD where pulling in libdataclasses as a dependency of the pybindings will
# cause corruption of the internal state of the extended_type_info database
# when classes in dataclasses are inherited from in certain circumstances I
# can't identify.
# Observed on: FreeBSD 10 + Boost 1.52 + clang 3.2 when loading mutineer
# (of all things)
icecube.icetray.load('dataclasses', False)

load_pybindings(__name__,__path__)


# Add some aliases for naming consistency
I3RecoPulseSeries = vector_I3RecoPulse
I3RecoHitSeries = vector_I3RecoHit
I3MCHitSeries = vector_I3MCHit

del icecube
