#!/bin/sh

HELP="serialization::compile-only-tests

This test is used to ensure that certain uses of the serialization library
either do or don't compile.

It accepts the following options:

  -h, --help                  Display this help message
  -q, --quiet                 Hide ouput from test compile commands
  -r [pattern],
  --run-tests [pattern]       Only run tests whose names match the given pattern
  -v, --verbose               Show ouput from test compile commands
                              (This is the default)
"

VERBOSE=1

# parse arguments

if [ "$#" -ne 0 ]; then
    while true ; do
        var=${1-END}; shift

        if [ "$var" = "END" ]; then break; fi

        if [ "$var" = "--help" -o "$var" = "-h" ]; then
            echo "$HELP"
            exit 0
        fi

        if [ "$var" = "-r" -o "$var" = "--run-tests" ]; then
            TEST_FILTER=$1; shift
            echo "Only running tests matching '$TEST_FILTER'"
        fi

        if [ "$var" = "-v" -o "$var" = "--verbose" ]; then
            VERBOSE=1
        fi

        if [ "$var" = "-q" -o "$var" = "--quiet" ]; then
            unset VERBOSE
        fi
    done
fi

PROJECT="serialization"

# make sure necessary inputs are set
echo ${I3_SRC?"I3_SRC is not set"} > /dev/null
echo ${I3_BUILD?"I3_BUILD is not set"} > /dev/null
echo ${CMAKE_CXX_COMPILER?"CMAKE_CXX_COMPILER is not set"} > /dev/null

# fetch the necessary compiler flags
CXX_FLAGS=`${I3_BUILD}/bin/icetray-config $PROJECT`

# GNU ld is retarded and demands symbols from python which are ultimately never used, 
# due to cmake mixing boost_python in everwhere whether it is needed or not. 
# For now, manually link in python to shut it up.
CMAKECACHE=${I3_BUILD}/CMakeCache.txt
if [ ! -f "$CMAKECACHE" ]; then
    echo "CMakeCache.txt not found; expected location was $CMAKECACHE" 1>&2
    exit 1
fi
PYTHON_LIB_DIR=`grep -m 1 ^PYTHON_LIBRARY:FILEPATH $CMAKECACHE | sed -e's|^.*:FILEPATH=||' -e's|/[^/]*$||'`
PYTHON_LIB=`grep -m 1 ^PYTHON_LIBRARY:FILEPATH $CMAKECACHE | sed -e's|^.*:FILEPATH=.*/lib||' -e's|\.[^.]*$||'`
CXX_FLAGS="$CXX_FLAGS -L${PYTHON_LIB_DIR} -l${PYTHON_LIB}"

TEST_DIR="${I3_SRC}/${PROJECT}/private/test"

should_skip() {
    if [ -z "$TEST_FILTER" ] ; then
        return 1
    fi
    echo "$test" | grep -qv "$TEST_FILTER"
    return $?
}

compile() {
    COMPILE_COMMAND="$CMAKE_CXX_COMPILER $CXX_FLAGS ${TEST_DIR}/$test ${TEST_DIR}/main.cxx -o /dev/null"
    if [ -z "$VERBOSE" ] ; then # not verbose
        $COMPILE_COMMAND > /dev/null 2>&1
    else # verbose mode
        $COMPILE_COMMAND
    fi
}

expect_compile() {
    test=$1
    if should_skip ; then
        return
    fi
    echo "Compiling $test; success expected"
    if [ ! -f ${TEST_DIR}/$1 ]; then
        echo "Source code file ${TEST_DIR}/$test not found" 1>&2
        exit 1
    fi
    compile
    if [ "$?" -ne 0 ]; then
        echo "ERROR: $test does not compile when it should" 1>&2
        exit 1
    else
        echo "    compilation succeeded"
    fi
}

expect_not_compile() {
    test=$1
    if should_skip ; then
        return
    fi
    echo "Compiling $test; failure expected"
    if [ ! -f ${TEST_DIR}/$1 ]; then
        echo "Source code file ${TEST_DIR}/$test not found" 1>&2
        exit 1
    fi
    if compile ; then
        echo "ERROR: $test compiles when it should not" 1>&2
        exit 1
    else
        echo "    compilation failed"
    fi
}

####################################

expect_not_compile "test_check.cpp"

expect_compile "test_const_pass.cpp"

# these tests' names suggest that they should generate warnings, but they don't
# and there is no obvious reason that they should
expect_compile "test_const_save_warn1_nvp.cpp"
expect_compile "test_const_save_warn1.cpp"
expect_compile "test_const_save_warn2_nvp.cpp"
expect_compile "test_const_save_warn2.cpp"
expect_compile "test_const_save_warn3_nvp.cpp"
expect_compile "test_const_save_warn3.cpp"
expect_compile "test_const_save_warn4_nvp.cpp"
expect_compile "test_const_save_warn4.cpp"

expect_not_compile "test_not_serializable.cpp"

# this test is supposed to generate warnings, but doesn't
expect_compile "test_static_warning.cpp"

expect_not_compile "test_const_load_fail1_nvp.cpp"
expect_not_compile "test_const_load_fail1.cpp"
expect_not_compile "test_const_load_fail2_nvp.cpp"
expect_not_compile "test_const_load_fail2.cpp"
expect_not_compile "test_const_load_fail3_nvp.cpp"
expect_not_compile "test_const_load_fail3.cpp"

expect_not_compile "test_traits_fail.cpp"
expect_compile "test_traits_pass.cpp"
