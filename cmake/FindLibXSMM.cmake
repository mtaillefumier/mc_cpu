include(FindPackageHandleStandardArgs)
include(mc_cpu_utils)
find_package(PkgConfig REQUIRED)

mc_cpu_set_default_paths(LIBXSMM "LibXSMM")
set(mc_cpu_LIBXSMMEXT_ROOT "${mc_cpu_LIBXSMM_PREFIX}")
set(mc_cpu_LIBXSMMF_ROOT "${mc_cpu_LIBXSMM_PREFIX}")
set(mc_cpu_LIBXSMMNOBLAS_ROOT "${mc_cpu_LIBXSMM_PREFIX}")

if(PKG_CONFIG_FOUND)
  pkg_check_modules(mc_cpu_LIBXSMM IMPORTED_TARGET GLOBAL libxsmm)
  pkg_check_modules(mc_cpu_LIBXSMMEXT IMPORTED_TARGET GLOBAL libxsmmext)
  pkg_check_modules(mc_cpu_LIBXSMMF IMPORTED_TARGET GLOBAL libxsmmf)
  pkg_check_modules(mc_cpu_LIBXSMMNOBLAS IMPORTED_TARGET GLOBAL libxsmmnoblas)

  # i need to do it twice because of dbcsr build option
  pkg_check_modules(LIBXSMM QUIET IMPORTED_TARGET GLOBAL libxsmm)
  pkg_check_modules(LIBXSMMEXT QUIET IMPORTED_TARGET GLOBAL libxsmmext)
  pkg_check_modules(LIBXSMMF QUIET IMPORTED_TARGET GLOBAL libxsmmf)
  pkg_check_modules(LIBXSMMNOBLAS QUIET IMPORTED_TARGET GLOBAL libxsmmnoblas)
endif()

if(NOT mc_cpu_LIBXSMM_FOUND)
  # Reset after pkg_check_modules side effects
  foreach(__lib xsmm xsmmf xsmmext xsmmnoblas)
    string(TOUPPER "LIB${__lib}" __lib_search_up)
    if(NOT mc_cpu_${__lib_search_up}_FOUND)
      mc_cpu_find_libraries(${__lib_search_up} ${__lib})
    endif()
  endforeach()
endif()

if(NOT mc_cpu_LIBXSMM_INCLUDE_DIRS)
  mc_cpu_include_dirs(LIBXSMM "libxsmm.h;include/libxsmm.h")
endif()

if(mc_cpu_LIBXSMM_INCLUDE_DIRS)
  find_package_handle_standard_args(
    LibXSMM
    DEFAULT_MSG
    mc_cpu_LIBXSMM_INCLUDE_DIRS
    mc_cpu_LIBXSMMNOBLAS_LINK_LIBRARIES
    mc_cpu_LIBXSMMEXT_LINK_LIBRARIES
    mc_cpu_LIBXSMMF_LINK_LIBRARIES
    mc_cpu_LIBXSMM_LINK_LIBRARIES)
else()
  find_package_handle_standard_args(
    LibXSMM DEFAULT_MSG mc_cpu_LIBXSMMNOBLAS_LINK_LIBRARIES
    mc_cpu_LIBXSMMEXT_LINK_LIBRARIES mc_cpu_LIBXSMMF_LINK_LIBRARIES
    mc_cpu_LIBXSMM_LINK_LIBRARIES)
endif()

if(NOT TARGET mc_cpu::LibXSMM::libxsmm)
  foreach(__lib libxsmm libxsmmf libxsmmext libxsmmnoblas)
    string(TOUPPER "mc_cpu_${__lib}" __lib_search_up)
    if(${__lib_search_up}_FOUND AND NOT TARGET mc_cpu::LibXSMM::${__lib})
      add_library(mc_cpu::LibXSMM::${__lib} INTERFACE IMPORTED)
    endif()

    set_target_properties(
      mc_cpu::LibXSMM::${__lib} PROPERTIES INTERFACE_LINK_LIBRARIES
                                         "${${__lib_search_up}_LINK_LIBRARIES}")

    if(mc_cpu_LIBXSMM_INCLUDE_DIRS)
      set_target_properties(
        mc_cpu::LibXSMM::${__lib}
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                   "${mc_cpu_LIBXSMM_INCLUDE_DIRS};${mc_cpu_LIBXSMM_PREFIX}/include"
      )
    endif()
  endforeach()
endif()

mark_as_advanced(
  mc_cpu_LIBXSMM_INCLUDE_DIRS mc_cpu_LIBXSMMNOBLAS_LINK_LIBRARIES
  mc_cpu_LIBXSMMEXT_LINK_LIBRARIES mc_cpu_LIBXSMMF_LINK_LIBRARIES
  mc_cpu_LIBXSMM_LINK_LIBRARIES)
