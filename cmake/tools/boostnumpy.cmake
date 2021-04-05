
tooldef(boostnumpy
    include
    boost/python/numpy.hpp
    lib
    NONE # The bin directory is n/a, placeholder
    boost_numpy${Python_VERSION_MAJOR}${Python_VERSION_MINOR}
)

# If we didn't find the library, it may be that we just guessed the 
# wrong python version suffix, so repeat with some likely variants.
IF (NOT BOOSTNUMPY_FOUND)
    tooldef(boostnumpy
        include
        boost/python/numpy.hpp
        lib
        NONE # The bin directory is n/a, placeholder
        boost_numpy${Python_VERSION_MAJOR} # Try with simple suffix
)
ENDIF()

IF (NOT BOOSTNUMPY_FOUND)
    tooldef(boostnumpy
        include
        boost/python/numpy.hpp
        lib
        NONE # The bin directory is n/a, placeholder
        boost_numpy # Try with no suffix
)
ENDIF()

# boost::numpy can't work without the Numpy C API
IF (BOOSTNUMPY_FOUND)
	LIST(APPEND BOOSTNUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR})
	LIST(APPEND BOOSTNUMPY_LIBRARIES ${NUMPY_LIBRARIES})
ELSE()
	colormsg(RED "+-- boostnumpy NOT found")
ENDIF (BOOSTNUMPY_FOUND)
