# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(FindPackageHandleStandardArgs)

# check for blas first. Most of the vendor libraries bundle lapack and blas in
# the same library. If so the FindBlas.cmake module will contain this
# information already and the information will be included in the blas target
#
# This solution might not good enough though.

find_package(PkgConfig)
find_package(Blas REQUIRED)

if(mc_cpu_BLAS_FOUND)
  # LAPACK in the Intel MKL 10+ library?
  if(mc_cpu_BLAS_VENDOR MATCHES "MKL|OpenBLAS|Armpl|SCI|FlexiBLAS|NVHPC")
    # we just need to create the interface that's all
    set(mc_cpu_LAPACK_FOUND TRUE)
    get_target_property(mc_cpu_LAPACK_INCLUDE_DIRS mc_cpu::BLAS::blas
                        INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(mc_cpu_LAPACK_LINK_LIBRARIES mc_cpu::BLAS::blas
                        INTERFACE_LINK_LIBRARIES)
  else()
    # we might get lucky to find a pkgconfig package for lapack (fedora provides
    # one for instance)
    if(PKG_CONFIG_FOUND)
      pkg_check_modules(mc_cpu_LAPACK lapack)
    endif()

    if(NOT mc_cpu_LAPACK_FOUND)
      find_library(
        mc_cpu_LAPACK_LINK_LIBRARIES
        NAMES "lapack" "lapack64"
        PATH_SUFFIXES "openblas" "openblas64" "openblas-pthread"
                      "openblas-openmp" "lib" "lib64"
        NO_DEFAULT_PATH)
    endif()
  endif()
endif()

# check if found
find_package_handle_standard_args(Lapack
                                  REQUIRED_VARS mc_cpu_LAPACK_LINK_LIBRARIES)

if(NOT TARGET mc_cpu::LAPACK::lapack)
  add_library(mc_cpu::LAPACK::lapack INTERFACE IMPORTED)
  add_library(mc_cpu::LAPACK::LAPACK ALIAS mc_cpu::LAPACK::lapack)
endif()
set_property(TARGET mc_cpu::LAPACK::lapack PROPERTY INTERFACE_LINK_LIBRARIES
                                                  ${mc_cpu_LAPACK_LINK_LIBRARIES})
if(mc_cpu_LAPACK_INCLUDE_DIRS)
  set_property(
    TARGET mc_cpu::LAPACK::lapack PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                         ${mc_cpu_LAPACK_INCLUDE_DIRS})
endif()

# prevent clutter in cache
mark_as_advanced(mc_cpu_LAPACK_FOUND mc_cpu_LAPACK_LIBRARIES
                 mc_cpu_LAPACK_INCLUDE_DIRS)
