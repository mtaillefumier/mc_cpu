# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)

mc_cpu_set_default_paths(ATLAS "Atlas")

mc_cpu_find_libraries(ATLAS "atlas")
mc_cpu_include_dirs(FFTW3 "cblas.h atlas/cblas.h")
# check if found
find_package_handle_standard_args(Atlas REQUIRED_VARS mc_cpu_ATLAS_INCLUDE_DIRS
                                                      mc_cpu_ATLAS_LINK_LIBRARIES)

# add target to link against
if(mc_cpu_ATLAS_FOUND AND NOT TARGET mc_cpu_ATLAS::atlas)
  if(NOT TARGET mc_cpu::BLAS::ATLAS::atlas)
    add_library(mc_cpu::BLAS::ATLAS::atlas INTERFACE IMPORTED)
  endif()
  set_property(TARGET mc_cpu::BLAS::ATLAS::atlas
               PROPERTY INTERFACE_LINK_LIBRARIES ${mc_cpu_ATLAS_LINK_LIBRARIES})
  set_property(
    TARGET mc_cpu::BLAS::ATLAS::atlas PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                             ${mc_cpu_ATLAS_INCLUDE_DIRS})
endif()

# prevent clutter in cache
mark_as_advanced(mc_cpu_ATLAS_FOUND mc_cpu_ATLAS_LINK_LIBRARIES
                 mc_cpu_ATLAS_INCLUDE_DIRS)
