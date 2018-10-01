
tooldef(boostnumpy
    include
    boost/numpy.hpp
    lib
    NONE # The bin directory is n/a, placeholder
    boost_numpy
)

# boost::numpy can't work without the Numpy C API
IF (BOOSTNUMPY_FOUND)
	LIST(APPEND BOOSTNUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR})
	LIST(APPEND BOOSTNUMPY_LIBRARIES ${NUMPY_LIBRARIES})
ENDIF (BOOSTNUMPY_FOUND)
