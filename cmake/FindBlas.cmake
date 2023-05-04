# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

if(NOT
   (CMAKE_C_COMPILER_LOADED
    OR CMAKE_CXX_COMPILER_LOADED
    OR CMAKE_Fortran_COMPILER_LOADED))
  message(FATAL_ERROR "FindBLAS requires Fortran, C, or C++ to be enabled.")
endif()

set(mc_cpu_BLAS_VENDOR_LIST
    "auto"
    "MKL"
    "OpenBLAS"
    "SCI"
    "GenericBLAS"
    "Armpl"
    "FlexiBLAS"
    "Atlas"
    "NVHPCBlas"
    "CUSTOM")

set(__BLAS_VENDOR_LIST ${mc_cpu_BLAS_VENDOR_LIST})
list(REMOVE_ITEM __BLAS_VENDOR_LIST "auto")
list(REMOVE_ITEM __BLAS_VENDOR_LIST "CUSTOM")

# set(mc_cpu_BLAS_VENDOR "auto" CACHE STRING "Blas library for computations on
# host")
set_property(CACHE mc_cpu_BLAS_VENDOR PROPERTY STRINGS ${mc_cpu_BLAS_VENDOR_LIST})

if(NOT ${mc_cpu_BLAS_VENDOR} IN_LIST mc_cpu_BLAS_VENDOR_LIST)
  message(FATAL_ERROR "Invalid Host BLAS backend")
endif()

set(mc_cpu_BLAS_THREAD_LIST "sequential" "thread" "gnu-thread" "intel-thread"
                          "tbb-thread" "openmp")

set(mc_cpu_BLAS_THREADING
    "sequential"
    CACHE STRING "threaded blas library")
set_property(CACHE mc_cpu_BLAS_THREADING PROPERTY STRINGS
                                                ${mc_cpu_BLAS_THREAD_LIST})

if(NOT ${mc_cpu_BLAS_THREADING} IN_LIST mc_cpu_BLAS_THREAD_LIST)
  message(FATAL_ERROR "Invalid threaded BLAS backend")
endif()

set(mc_cpu_BLAS_INTERFACE_BITS_LIST "32bits" "64bits")
set(mc_cpu_BLAS_INTERFACE
    "32bits"
    CACHE STRING
          "32 bits integers are used for indices, matrices and vectors sizes")
set_property(CACHE mc_cpu_BLAS_INTERFACE
             PROPERTY STRINGS ${mc_cpu_BLAS_INTERFACE_BITS_LIST})

if(NOT ${mc_cpu_BLAS_INTERFACE} IN_LIST mc_cpu_BLAS_INTERFACE_BITS_LIST)
  message(
    FATAL_ERROR
      "Invalid parameters. Blas and lapack can exist in two flavors 32 or 64 bits interfaces (relevant mostly for mkl)"
  )
endif()

set(mc_cpu_BLAS_FOUND FALSE)

# first check for a specific implementation if requested

if(NOT mc_cpu_BLAS_VENDOR MATCHES "auto|CUSTOM")
  find_package(${mc_cpu_BLAS_VENDOR} REQUIRED)
  if(TARGET mc_cpu::BLAS::${mc_cpu_BLAS_VENDOR}::blas)
    get_target_property(
      mc_cpu_BLAS_INCLUDE_DIRS mc_cpu::BLAS::${mc_cpu_BLAS_VENDOR}::blas
      INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(
      mc_cpu_BLAS_LINK_LIBRARIES mc_cpu::BLAS::${mc_cpu_BLAS_VENDOR}::blas
      INTERFACE_LINK_LIBRARIES)
    set(mc_cpu_BLAS_FOUND TRUE)
  endif()
else()
  if(mc_cpu_BLAS_VENDOR MATCHES "CUSTOM" AND NOT DEFINED mc_cpu_BLAS_LINK_LIBRARIES)
    message(
      FATAL_ERROR
        "Setting mc_cpu_BLAS_VENDOR=CUSTOM imply setting mc_cpu_BLAS_LINK_LIBRARIES\n and mc_cpu_LAPACK_LINK_LIBRARIES to the right libraries. See the README_cmake.md for more details"
    )
  endif()

  if(DEFINED mc_cpu_BLAS_LINK_LIBRARIES)
    set(mc_cpu_BLAS_FOUND TRUE)
  else()
    # search for any blas implementation and exit immediately if one is found.
    # we could also give a full list of found implementation and let the user
    # choose which implementation to use
    foreach(_libs ${__BLAS_VENDOR_LIST})
      # I exclude the first item of the list
      find_package(${_libs})
      if(TARGET mc_cpu::BLAS::${_libs}::blas)
        get_target_property(mc_cpu_BLAS_INCLUDE_DIRS mc_cpu::BLAS::${_libs}::blas
                            INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(mc_cpu_BLAS_LINK_LIBRARIES mc_cpu::BLAS::${_libs}::blas
                            INTERFACE_LINK_LIBRARIES)
        set(mc_cpu_BLAS_VENDOR "${_libs}")
        set(mc_cpu_BLAS_FOUND TRUE)
        break()
      endif()
    endforeach()
  endif()
endif()

# we exclude the mc_cpu_BLAS_INCLUDE_DIRS from the list of mandatory variables as
# having the fortran interface is usually enough. C, C++ and others languages
# might require this information though

find_package_handle_standard_args(
  Blas REQUIRED_VARS mc_cpu_BLAS_LINK_LIBRARIES mc_cpu_BLAS_VENDOR mc_cpu_BLAS_FOUND)

if(NOT TARGET mc_cpu::BLAS::blas)
  add_library(mc_cpu::BLAS::blas INTERFACE IMPORTED)
endif()

set_target_properties(mc_cpu::BLAS::blas PROPERTIES INTERFACE_LINK_LIBRARIES
                                                  "${mc_cpu_BLAS_LINK_LIBRARIES}")

if(mc_cpu_BLAS_INCLUDE_DIRS)
  set_target_properties(
    mc_cpu::BLAS::blas PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                "${mc_cpu_BLAS_INCLUDE_DIRS}")
endif()

mark_as_advanced(mc_cpu_BLAS_INCLUDE_DIRS)
mark_as_advanced(mc_cpu_BLAS_LINK_LIBRARIES)
mark_as_advanced(mc_cpu_BLAS_VENDOR)
mark_as_advanced(mc_cpu_BLAS_FOUND)
