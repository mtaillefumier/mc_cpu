# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

find_package(PkgConfig)
include(mc_cpu_utils)
include(FindPackageHandleStandardArgs)

mc_cpu_set_default_paths(BLIS "BLIS")

if(DEFINED AOCL_ROOT)
  list(mc_cpu_BLIS_ROOT "${AOCL_ROOT}" "$ENV{AOCL_ROOT}")
endif()

# one day blis will have a pkg-config file
if(PKG_CONFIG_FOUND)
  pkg_check_modules(BLIS IMPORTED_TARGET GLOBAL blis)
endif()

if(NOT mc_cpu_BLIS_FOUND)
  mc_cpu_find_libraries(BLIS "blis")
endif()

if(NOT mc_cpu_BLIS_INCLUDE_DIRS)
  mc_cpu_include_dirs(BLIS "blis.h")
endif()

# check if found
if(mc_cpu_BLIS_INCLUDE_DIRS)
  find_package_handle_standard_args(
    BLIS REQUIRED_VARS mc_cpu_BLIS_FOUND mc_cpu_BLIS_INCLUDE_DIRS
                       mc_cpu_BLIS_LINK_LIBRARIES)
else()
  find_package_handle_standard_args(Blis REQUIRED_VARS mc_cpu_BLIS_FOUND
                                                       mc_cpu_BLIS_LINK_LIBRARIES)
endif()

# add target to link against
if(mc_cpu_BLIS_FOUND)
  if(NOT TARGET mc_cpu::BLAS::Blis::blis)
    add_library(mc_cpu::BLAS::Blis::blis INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::Blis::blas alias mc_cpu::BLAS::Blis::blis)
  endif()

  set_property(TARGET mc_cpu::BLAS::Blis::blis
               PROPERTY INTERFACE_LINK_LIBRARIES ${mc_cpu_BLIS_LINK_LIBRARIES})

  if(BLIS_INCLUDE_DIRS)
    set_property(
      TARGET mc_cpu::BLAS::Blis::blis PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                             ${mc_cpu_BLIS_INCLUDE_DIRS})
  endif()
endif()

# prevent clutter in cache
mark_as_advanced(mc_cpu_BLIS_FOUND mc_cpu_BLIS_LINK_LIBRARIES
                 mc_cpu_BLIS_INCLUDE_DIRS)
