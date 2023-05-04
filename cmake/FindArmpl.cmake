# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)

find_package(PkgConfig)

mc_cpu_set_default_paths(ARMPL "Armpl")

message(STATUS "Armpl prefix : ${mc_cpu_ARMPL_PREFIX}")
foreach(
  _var
  armpl
  amath
  astring
  armpl_ilp64
  armpl_lp64
  armpl_ilp64_mp
  armpl_lp64_mp)
  string(TOUPPER "${_var}" _var_up)
  mc_cpu_find_libraries("${_var_up}" "${_var}")
endforeach()

mc_cpu_include_dirs(ARMPL "armpl.h")

# Check for 64bit Integer support
if(mc_cpu_BLAS_INTERFACE MATCHES "64bits")
  set(mc_cpu_BLAS_armpl_LIB "ARMPL_ILP64")
else()
  set(mc_cpu_BLAS_armpl_LIB "ARMPL_LP64")
endif()

# Check for OpenMP support, VIA BLAS_VENDOR of Arm_mp or Arm_ipl64_mp
if(mc_cpu_BLAS_THREADING MATCHES "openmp")
  set(mc_cpu_BLAS_armpl_LIB "${mc_cpu_BLAS_armpl_LIB}_MP")
endif()

# check if found
find_package_handle_standard_args(
  Armpl
  REQUIRED_VARS
    mc_cpu_ARMPL_INCLUDE_DIRS mc_cpu_ARMPL_LP64_LINK_LIBRARIES
    mc_cpu_ARMPL_LP64_MP_LINK_LIBRARIES mc_cpu_ARMPL_ILP64_LINK_LIBRARIES
    mc_cpu_ARMPL_ILP64_MP_LINK_LIBRARIES)

# add target to link against
if(NOT TARGET Armpl::armpl)
  add_library(mc_cpu::BLAS::Armpl::armpl INTERFACE IMPORTED)
  # now define an alias to the target library
  add_library(mc_cpu::BLAS::Armpl::blas ALIAS mc_cpu::BLAS::Armpl::armpl)
endif()

# we need to iniitialize the targets of each individual libraries only once.
foreach(_var armpl_ilp64 armpl_lp64 armpl_ilp64_mp armpl_lp64_mp)
  if(NOT TARGET mc_cpu::BLAS::Armpl::${_var})
    string(TOUPPER "${_var}" _var_up)
    add_library(mc_cpu::BLAS::Armpl::${_var} INTERFACE IMPORTED)
    set_property(
      TARGET mc_cpu::BLAS::Armpl::${_var} PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                                 ${mc_cpu_ARMPL_INCLUDE_DIRS})
    set_property(
      TARGET mc_cpu::BLAS::Armpl::${_var}
      PROPERTY INTERFACE_LINK_LIBRARIES "${mc_cpu_${_var_up}_LINK_LIBRARIES}")
  endif()
endforeach()

set_property(TARGET mc_cpu::BLAS::Armpl::armpl
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${mc_cpu_ARMPL_INCLUDE_DIRS})
set_property(
  TARGET mc_cpu::BLAS::Armpl::armpl
  PROPERTY INTERFACE_LINK_LIBRARIES
           "${mc_cpu_${mc_cpu_BLAS_armpl_LIB}_LINK_LIBRARIES}")

mark_as_advanced(mc_cpu_ARMPL_FOUND mc_cpu_ARMPL_LINK_LIBRARIES
                 mc_cpu_ARMPL_INCLUDE_DIRS)
