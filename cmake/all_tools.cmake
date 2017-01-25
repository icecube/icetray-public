#
#  These guys are in subdirectory 'tools'
#
set(ALL_TOOLS pthread root python boost boostnumpy
    blas lapack gsl sprng sla libarchive
    mysql bdb mpi suitesparse ZThread omniORB ncurses cdk
    cfitsio healpix qt4 hdf5 minuit2 clhep geant4 zlib
    opencl gmp log4cpp xml2 genie zmq doxygen
    multinest nlopt hepmc
    ${I3_EXTRA_TOOLS}   # add the extra tools and dedupe
    )
list(REMOVE_DUPLICATES ALL_TOOLS)
