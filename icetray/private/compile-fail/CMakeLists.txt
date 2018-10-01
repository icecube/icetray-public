set(COMPILE_FAIL_TESTS fail frame-get frame-get-nonconst-ref)

foreach(TEST ${COMPILE_FAIL_TESTS})
  i3_test_compile_fail(${TEST} ${TEST}.cxx)
endforeach(TEST ${COMPILE_FAIL_TESTS})


