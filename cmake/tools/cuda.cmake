## CUDA is detected already in ../config.cmake via enable_language()
## this just just helps setup some convenient variables

## FindCUDAToolkit was introduced in 3.17 and we're not really
## supporing clang-compiled-CUDA yet

## We could do more here for cmake < 3.17, but only clsim (and ppc)
## use these variables and only CUDAToolkit_VERSION_MAJOR

if(CMAKE_CUDA_COMPILER MATCHES "nvcc")
  colormsg("")
  colormsg(HICYAN "CUDA")
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.17")
    find_package(CUDAToolkit REQUIRED)
  else()
    if(CMAKE_CUDA_COMPILER_VERSION MATCHES [=[([0-9]+)\.([0-9]+)\.([0-9]+)]=])
      set(CUDAToolkit_VERSION_MAJOR "${CMAKE_MATCH_1}")
      set(CUDAToolkit_VERSION_MINOR "${CMAKE_MATCH_2}")
      set(CUDAToolkit_VERSION_PATCH "${CMAKE_MATCH_3}")
      set(CUDAToolkit_VERSION "${CMAKE_CUDA_COMPILER_VERSION}")
    endif()
  endif()
  found_ok("  version: ${CUDAToolkit_VERSION}")
  found_ok("   binary: ${CMAKE_CUDA_COMPILER}")
 endif()
