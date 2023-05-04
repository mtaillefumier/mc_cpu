#!-------------------------------------------------------------------------------------------------!
#!   CP2K: A general program to perform molecular dynamics simulations                             !
#!   Copyright 2000-2023 CP2K developers group <https://cp2k.org>                                  !
#!                                                                                                 !
#!   SPDX-License-Identifier: GPL-2.0-or-later                                                     !
#!-------------------------------------------------------------------------------------------------!

cmake_minimum_required(VERSION 3.22)

# include our cmake snippets
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# =================================================================================================
# REQUIRE OUT-OF-SOURCE BUILDS
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
  message(
    FATAL_ERROR
      "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory."
  )
endif()

# =================================================================================================
# PROJECT AND VERSION
include(CMakeDependentOption)
#include(GitSubmodule)

cmake_policy(SET CMP0048 NEW)

set(VERSION_MAJOR 0)
# anything above 12 is to indicate that it is devel-branch. The git hash commit
# is the only important information to give. It is retrieved few lines below.
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

project(
  mc_cpu
  DESCRIPTION "small cpu code for doing montecarlo simulations"
  HOMEPAGE_URL "none"
  VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
  LANGUAGES C CXX)

set(mc_cpu_APIVERSION ${mc_cpu_VERSION_MAJOR}.${cmc_cpu_VERSION_MINOR})

if(NOT DEFINED CMAKE_CUDA_STANDARD)
  set(CMAKE_CUDA_STANDARD 14)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# set language and standard
set(CMAKE_CXX_STANDARD 17)

set(mc_cpu_BLAS_VENDOR
  "auto"
  CACHE STRING "Blas library for computations on host")

find_package(umpire REQUIRED)
find_package(PkgConfig)
find_package(OpenMP)
# ##############################################################################
# Define the paths for static libraries and executables
# ##############################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${mc_cpu_BINARY_DIR}/lib
  CACHE PATH "Single output directory for building all libraries.")

set(MC_CPU_BLAS_VENDOR
  "auto"
  CACHE STRING "Blas library for computations on host")


# Python
#
# this module looks preferably for version 3 of Python. If not found, version 2
# is searched. In CMake 3.15, if a python virtual environment is activated, it
# will search the virtual environment for a python interpreter before searching
# elsewhere in the system. In CMake <3.15, the system is searched before the
# virtual environment.


# MPIget_property(REQUIRED_MPI_COMPONENTS GLOBAL PROPERTY ENABLED_LANGUAGES)
#list(REMOVE_ITEM REQUIRED_MPI_COMPONENTS CUDA) # CUDA does not have an MPI
# component
find_package(
  MPI
  #COMPONENTS ${REQUIRED_MPI_COMPONENTS}
  REQUIRED)

find_package(GSL REQUIRED)

# BLAS & LAPACK, PkgConfig
find_package(Lapack REQUIRED) # also calls find_package(BLAS)

# SMM (Small Matrix-Matrix multiplication)
#find_package(LibXSMM REQUIRED)
#message(STATUS "-- Using libxsmm for Small Matrix Multiplication")


# =================================================================================================
# PACKAGE DISCOVERY (compiler configuration can impact package discovery)

find_package(OpenMP REQUIRED COMPONENTS C CXX)


# OPTION HANDLING

# make sure that the default build type is RELEASE
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING
            "Choose the type of build, options are: Debug Release Coverage."
            FORCE)
  # set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "Coverage")
endif()

#include(GNUInstallDirs)

add_executable(Shuriken Shuriken.cc)
set_target_properties(Shuriken PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
  LINKER_LANGUAGE "CXX")
target_compile_features(Shuriken PUBLIC cxx_std_17)
target_link_libraries(Shuriken PUBLIC ${GSL_LIBRARY} mc_cpu::BLAS::blas umpire MPI::MPI_CXX)
