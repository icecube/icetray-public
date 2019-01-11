set(COMPILE_TESTS ok frame-get frame-get-const-ref)

foreach(TEST ${COMPILE_TESTS})
  i3_test_compile(${TEST} ${TEST}.cxx)
endforeach(TEST ${COMPILE_TESTS})


