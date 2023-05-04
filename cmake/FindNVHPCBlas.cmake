# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)

mc_cpu_set_default_paths(mc_cpu_NVHPC "NVHPC")

find_library(
  mc_cpu_NVHPC_BLAS_LP64
  NAMES blas_lp64
  PATHS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "lib" "lib64")

find_library(
  mc_cpu_NVHPC_BLAS_ILP64
  NAMES blas_ilp64
  PATHS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "lib" "lib64")

find_library(
  mc_cpu_NVHPC_LAPACK_LP64
  NAMES lapack_lp64
  PATHS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "lib" "lib64")

find_library(
  mc_cpu_NVHPC_LAPACK_ILP64
  NAMES lapack_ilp64
  PATHS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "lib" "lib64")

find_path(
  mc_cpu_NVHPC_BLAS_INCLUDE_DIRS_lp64
  NAMES cblas.h
  PATHS "${mc_cpu_NVHPC_ROOT}"
  HINTS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "include" "include/lp64" "lp64")

find_path(
  mc_cpu_NVHPC_BLAS_INCLUDE_DIRS_ilp64
  NAMES cblas.h
  PATHS "${mc_cpu_NVHPC_ROOT}"
  HINTS "${mc_cpu_NVHPC_ROOT}"
  PATH_SUFFIXES "include" "include/ilp64" "ilp64")

find_package_handle_standard_args(
  NVHPCBlas
  DEFAULT_MSG
  mc_cpu_NVHPC_INCLUDE_DIRS_ipl64
  mc_cpu_NVHPC_BLAS_INCLUDE_DIRS_lp64
  mc_cpu_NVHPC_BLAS_ILP64
  mc_cpu_NVHPC_BLAS_LP64
  mc_cpu_NVHPC_LAPACK_ILP64
  mc_cpu_NVHPC_LAPACK_LP64)

set(mc_cpu_BLAS_VENDOR "NVHPCBlas")
set(mc_cpu_NVHPCBLAS_FOUND "ON")

if(NOT TARGET mc_cpu::BLAS::NVHPCBlas::nvhpcblas)
  add_library(mc_cpu::BLAS::NVHPCBlas::nvhpcblas INTERFACE IMPORTED)
  add_library(mc_cpu::BLAS::NVHPCBlas::blas ALIAS
              mc_cpu::BLAS::NVHPCBlas::nvhpcblas)
endif()

if(mc_cpu_BLAS_INTERFACE MATCHES "64bits")
  set(mc_cpu_NVHPC_BLAS_LINK_LIBRARIES
      "${mc_cpu_NVHPC_LAPACK_ILP64} ${mc_cpu_NVHPC_BLAS_ILP64}")
  set(mc_cpu_NVHPC_BLAS_INCLUDE_DIRS "${mc_cpu_NVHPC_INCLUDE_DIRS_ipl64}")
else()
  set(mc_cpu_NVHPC_BLAS_LINK_LIBRARIES "${mc_cpu_NVHPC_LAPACK_LP64}
        ${mc_cpu_NVHPC_BLAS_LP64}")
  set(mc_cpu_NVHPC_BLAS_INCLUDE_DIRS "${mc_cpu_NVHPC_INCLUDE_DIRS_pl64}")
endif()

set_target_properties(
  mc_cpu::BLAS::NVHPCBlas::nvhpcblas
  PROPERTIES INTERFACE_LINK_LIBRARIES "${mc_cpu_NVHPC_BLAS_LINK_LIBRARIES}"
             INTERFACE_INCLUDE_DIRECTORIES "${mc_cpu_NVHPC_BLAS_INCLUDE_DIRS}")

mark_as_advanced(mc_cpu_NVHPCBLAS_FOUND mc_cpu_NVHPC_BLAS_INCLUDE_DIRS
                 mc_cpu_NVHPC_BLAS_LINK_LIBRARIES mc_cpu_BLAS_VENDOR)
