# Copyright (c) 2022- ETH Zurich
#
# authors : Mathieu Taillefumier

include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)

mc_cpu_set_default_paths(FLEXIBLAS "FlexiBLAS")

# try first with pkg-config
find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
  pkg_check_modules(mc_cpu_FLEXIBLAS IMPORTED_TARGET GLOBAL flexiblas)
endif()

# manual; search
if(NOT mc_cpu_FLEXIBLAS_FOUND)
  mc_cpu_find_libraries(FLEXIBLAS "flexiblas")
endif()

# search for include directories anyway
if(NOT mc_cpu_FLEXIBLAS_INCLUDE_DIRS)
  mc_cpu_include_dirs(FLEXIBLAS "flexiblas.h")
endif()

find_package_handle_standard_args(
  FlexiBLAS DEFAULT_MSG mc_cpu_FLEXIBLAS_INCLUDE_DIRS
  mc_cpu_FLEXIBLAS_LINK_LIBRARIES)

if(NOT mc_cpu_FLEXIBLAS_FOUND)
  set(mc_cpu_BLAS_VENDOR "FlexiBLAS")
endif()

if(mc_cpu_FLEXIBLAS_FOUND)
  if(NOT TARGET mc_cpu::BLAS::FlexiBLAS::flexiblas)
    add_library(mc_cpu::BLAS::FlexiBLAS::flexiblas INTERFACE IMPORTED)
    add_library(mc_cpu::BLAS::FlexiBLAS::blas ALIAS
                mc_cpu::BLAS::FlexiBLAS::flexiblas)
  endif()
  set_target_properties(
    mc_cpu::BLAS::FlexiBLAS::flexiblas
    PROPERTIES INTERFACE_LINK_LIBRARIES "${mc_cpu_FLEXIBLAS_LINK_LIBRARIES}")
  if(mc_cpu_FLEXIBLAS_INCLUDE_DIRS)
    set_target_properties(
      mc_cpu::BLAS::FlexiBLAS::flexiblas
      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${mc_cpu_FLEXIBLAS_INCLUDE_DIRS}")
  endif()
  set(mc_cpu_BLAS_VENDOR "FlexiBLAS")
endif()

mark_as_advanced(mc_cpu_FLEXIBLAS_FOUND mc_cpu_FLEXIBLAS_INCLUDE_DIRS
                 mc_cpu_FLEXIBLAS_LINK_LIBRARIES mc_cpu_BLAS_VENDOR)
