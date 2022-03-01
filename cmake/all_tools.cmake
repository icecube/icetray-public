#
#  These guys are in subdirectory 'tools'
#
set(ALL_TOOLS pthread root python boost boostnumpy
    blas gsl sprng pal libarchive
    suitesparse ncurses cdk
    cfitsio healpix hdf5 minuit2 clhep geant4 zlib
    opencl gmp zmq
    multinest nlopt zstd fftw3 photospline
    ${I3_EXTRA_TOOLS}   # add the extra tools and dedupe
    )
list(REMOVE_DUPLICATES ALL_TOOLS)
