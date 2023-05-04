# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

# set paths to look for library from ROOT variables.If new policy is set,
# find_library() automatically uses them.
include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)

mc_cpu_set_default_paths(LIBSCI "SCI")

# we might need to change the logic a little here since the mc_cpu_find_library
# function expect to have mc_cpu_package_PREFIX set.

set(mc_cpu_LIBSCI_MP_ROOT "${mc_cpu_LIBSCI_ROOT}")
set(mc_cpu_LIBSCI_MPI_ROOT "${mc_cpu_LIBSCI_ROOT}")
set(mc_cpu_LIBSCI_MPI_MP_ROOT "${mc_cpu_LIBSCI_ROOT}")

set(_sci_lib "sci_gnu")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  set(_sci_lib "sci_intel")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(_sci_lib "sci_cray")
endif()

mc_cpu_find_libraries("LIBSCI" "${_sci_lib}")
mc_cpu_find_libraries("LIBSCI_MP" "${_sci_lib}_mp")
mc_cpu_find_libraries("LIBSCI_MPI" "${_sci_lib}_mpi")
mc_cpu_find_libraries("LIBSCI_MPI_MP" "${_sci_lib}_mpi_mp")
mc_cpu_include_dirs(LIBSCI "cblas.h")

# check if found
find_package_handle_standard_args(SCI REQUIRED_VARS mc_cpu_LIBSCI_INCLUDE_DIRS
                                                    mc_cpu_LIBSCI_LINK_LIBRARIES)

# add target to link against
if(mc_cpu_LIBSCI_FOUND)
  if(NOT TARGET mc_cpu::BLAS::SCI::sci)
    add_library(mc_cpu::BLAS::SCI::sci INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::SCI::sci_mpi INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::SCI::sci_mp INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::SCI::sci_mpi_mp INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::SCI::scalapack_link INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::SCI::blas INTERFACE IMPORTED)

    if(mc_cpu_LIBSCI_INCLUDE_DIRS)
      set_property(
        TARGET mc_cpu::BLAS::SCI::sci PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                             "${mc_cpu_LIBSCI_INCLUDE_DIRS}")
      set_property(
        TARGET mc_cpu::BLAS::SCI::sci_mp PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                                "${mc_cpu_LIBSCI_INCLUDE_DIRS}")
      set_property(
        TARGET mc_cpu::BLAS::SCI::sci_mpi PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                                 "${mc_cpu_LIBSCI_INCLUDE_DIRS}")
      set_property(
        TARGET mc_cpu::BLAS::SCI::sci_mpi_mp
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${mc_cpu_LIBSCI_INCLUDE_DIRS}")
    endif()

    set_property(
      TARGET mc_cpu::BLAS::SCI::sci PROPERTY INTERFACE_LINK_LIBRARIES
                                           ${mc_cpu_LIBSCI_LINK_LIBRARIES})
    set_property(
      TARGET mc_cpu::BLAS::SCI::sci_mp PROPERTY INTERFACE_LINK_LIBRARIES
                                              ${mc_cpu_LIBSCI_MP_LINK_LIBRARIES})
    set_property(
      TARGET mc_cpu::BLAS::SCI::sci_mpi
      PROPERTY INTERFACE_LINK_LIBRARIES ${mc_cpu_LIBSCI_MPI_LINK_LIBRARIES}
               mc_cpu::BLAS::SCI::sci)
    set_property(
      TARGET mc_cpu::BLAS::SCI::sci_mpi_mp
      PROPERTY INTERFACE_LINK_LIBRARIES ${mc_cpu_LIBSCI_MPI_MP_LINK_LIBRARIES}
               mc_cpu::BLAS::SCI::sci_mp)
    set_property(
      TARGET mc_cpu::BLAS::SCI::scalapack_link
      PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${mc_cpu_LIBSCI_INCLUDE_DIRS}")
  endif()

  if(mc_cpu_BLAS_THREADING MATCHES "sequential")
    set_property(TARGET mc_cpu::BLAS::SCI::blas PROPERTY INTERFACE_LINK_LIBRARIES
                                                       mc_cpu::BLAS::SCI::sci)
    set_property(TARGET mc_cpu::BLAS::SCI::scalapack_link
                 PROPERTY INTERFACE_LINK_LIBRARIES mc_cpu::BLAS::SCI::sci_mpi)
  else()
    set_property(TARGET mc_cpu::BLAS::SCI::blas PROPERTY INTERFACE_LINK_LIBRARIES
                                                       mc_cpu::BLAS::SCI::sci_mp)
    set_property(TARGET mc_cpu::BLAS::SCI::scalapack_link
                 PROPERTY INTERFACE_LINK_LIBRARIES mc_cpu::BLAS::SCI::sci_mpi_mp)
  endif()

  set(mc_cpu_BLAS_VENDOR "SCI")

endif()

# prevent clutter in cache
mark_as_advanced(
  mc_cpu_LIBSCI_FOUND
  mc_cpu_BLAS_VENDOR
  mc_cpu_LIBSCI_LINK_LIBRARIES
  mc_cpu_LIBSCI_MP_LINK_LIBRARIES
  mc_cpu_LIBSCI_MPI_LINK_LIBRARIES
  mc_cpu_LIBSCI_MPI_MP_LINK_LIBRARIES
  mc_cpu_LIBSCI_INCLUDE_DIRS)
