# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(mc_cpu_utils)
include(FindPackageHandleStandardArgs)

find_package(PkgConfig)

mc_cpu_set_default_paths(OPENBLAS "OpenBLAS")

if(PKG_CONFIG_FOUND)
  pkg_check_modules(mc_cpu_OPENBLAS IMPORTED_TARGET GLOBAL openblas)
endif()

# try the openblas module of openblas library Maybe we are lucky it is installed
# find_package(OPENBLAS QUIET)

if(NOT mc_cpu_OPENBLAS_FOUND)
  set(mc_cpu_OPENBLAS64_ROOT ${mc_cpu_OPENBLAS_ROOT})
  set(mc_cpu_OPENBLA_THREADS_ROOT ${mc_cpu_OPENBLAS_ROOT})
  mc_cpu_find_libraries(OPENBLAS "openblas")
  mc_cpu_find_libraries(OPENBLAS64 "openblas64")
  mc_cpu_find_libraries(OPENBLAS_THREADS "openblas_threads;openblas_omp")
  mc_cpu_find_libraries(OPENBLAS_THREADS64 "openblas64_threads;openblas64_omp")
endif()

mc_cpu_include_dirs(OPENBLAS "cblas.h")

# check if found
if(mc_cpu_OPENBLAS_INCLUDE_DIRS)
  find_package_handle_standard_args(
    OpenBLAS REQUIRED_VARS mc_cpu_OPENBLAS_INCLUDE_DIRS
                           mc_cpu_OPENBLAS_LINK_LIBRARIES)
else()
  find_package_handle_standard_args(OpenBLAS
                                    REQUIRED_VARS mc_cpu_OPENBLAS_LINK_LIBRARIES)
endif()

# add target to link against
if(mc_cpu_OPENBLAS_FOUND)
  if(NOT TARGET mc_cpu::BLAS::OpenBLAS::openblas)
    add_library(mc_cpu::BLAS::OpenBLAS::openblas INTERFACE IMPORTED)
  endif()
  set_property(
    TARGET mc_cpu::BLAS::OpenBLAS::openblas
    PROPERTY INTERFACE_LINK_LIBRARIES ${mc_cpu_OPENBLAS_LINK_LIBRARIES})
  if(mc_cpu_OPENBLAS_INCLUDE_DIRS)
    set_property(
      TARGET mc_cpu::BLAS::OpenBLAS::openblas
      PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${mc_cpu_OPENBLAS_INCLUDE_DIRS})
  endif()
  if(NOT TARGET mc_cpu::BLAS::OpenBLAS::blas)
    add_library(mc_cpu::BLAS::OpenBLAS::blas ALIAS mc_cpu::BLAS::OpenBLAS::openblas)
  endif()
  set(mc_cpu_BLAS_VENDOR "OpenBLAS")
endif()

# prevent clutter in cache
mark_as_advanced(mc_cpu_BLAS_VENDOR mc_cpu_OPENBLAS_FOUND
                 mc_cpu_OPENBLAS_LINK_LIBRARIES mc_cpu_OPENBLAS_INCLUDE_DIRS)
