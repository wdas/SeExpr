# Copyright Disney Enterprises, Inc.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License
# and the following modification to it: Section 6 Trademarks.
# deleted and replaced with:
#
# 6. Trademarks. This License does not grant permission to use the
# trade names, trademarks, service marks, or product names of the
# Licensor and its affiliates, except as required for reproducing
# the content of the NOTICE file.
#
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

## CMake compatibility issues: don't modify this, please!
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
## allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
## Use verbose make file
SET ( CMAKE_VERBOSE_MAKEFILE TRUE )

option(ENABLE_LLVM_BACKEND "Enable LLVM backend." OFF)
if (ENABLE_LLVM_BACKEND)
   #set(CMAKE_CXX_COMPILER clang++)
   # A convenience variable:
   set(LLVM_ROOT "" CACHE PATH "Root of LLVM install.")

   # A bit of a sanity check:
   if( NOT EXISTS ${LLVM_ROOT}/include/llvm )
     message(FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install")
   endif()

   # We incorporate the CMake features provided by LLVM:
   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake")
   include(LLVMConfig)

   # Now set the header and library paths:
   include_directories(BEFORE ${LLVM_INCLUDE_DIRS} )
   link_directories( ${LLVM_LIBRARY_DIRS} )
   add_definitions( ${LLVM_DEFINITIONS} )
   add_definitions(-DSEEXPR_ENABLE_LLVM)

   #todo infinite loop in this?
   #llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)
   find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config PATHS ${LLVM_ROOT}/bin)
   execute_process(
     COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs
     OUTPUT_VARIABLE REQ_LLVM_LIBRARIES 
     OUTPUT_STRIP_TRAILING_WHITESPACE
   )
   list(APPEND REQ_LLVM_LIBRARIES "-lncurses")
endif()

## project name & version
PROJECT( SeExpr )
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

## policies
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

## Setup platform specific helper defines build variants
IF(WIN32)
  ADD_DEFINITIONS (-DSEEXPR_WIN32)
ELSE(WIN32)
  ADD_DEFINITIONS (-Wall -Wextra -Wno-unused-parameter)
  ADD_DEFINITIONS (-pthread)

  SET( CMAKE_CXX_FLAGS "-fPIC -g -rdynamic --std=c++11 -msse4.1")
ENDIF(WIN32)

## Choose build options
# Disney specific method of choosing variant
IF("$ENV{FLAVOR}" STREQUAL "optimize")
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "type of build" FORCE)
ENDIF("$ENV{FLAVOR}" STREQUAL "optimize")
IF("$ENV{FLAVOR}" STREQUAL "debug")
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "type of build" FORCE)
ENDIF("$ENV{FLAVOR}" STREQUAL "debug")
# Set to release if nothing else defined
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(CMAKE_BUILD_TYPE STREQUAL "Release")
   SET(FLAVORDIR "optimize")
ELSE(CMAKE_BUILD_TYPE STREQUAL "Release")
   SET(FLAVORDIR "debug")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")


## Set install location
IF (NOT DEFINED CMAKE_INSTALL_PREFIX)
   EXECUTE_PROCESS(COMMAND sh -c "echo `uname`-`uname -r | cut -d'-' -f1`-`uname -m`" OUTPUT_VARIABLE VARIANT_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
   #EXECUTE_PROCESS(COMMAND uname OUTPUT_VARIABLE VARIANT_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
   SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/${VARIANT_DIRECTORY}-${FLAVORDIR}")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)

## Make modules able to see seexpr library
# Setup environment variable to link seexpr
SET( SEEXPR_LIBRARIES SeExpr2 )
SET( SEEXPR_EDITOR_LIBRARIES SeExprEditor2 )
# make it so seexpr can be found
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src/SeExpr )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src/SeExprEditor )

# Allowing to use lib64
IF (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR "lib")
ENDIF()


## Traverse subdirectories
ADD_SUBDIRECTORY (src/SeExpr)
ADD_SUBDIRECTORY (src/SeExprEditor)
ADD_SUBDIRECTORY (src/tests)
ADD_SUBDIRECTORY (src/py)
ADD_SUBDIRECTORY (src/utils)
ADD_SUBDIRECTORY (src/demos)

# TODO: re-enable
#ADD_SUBDIRECTORY (src/doc)


