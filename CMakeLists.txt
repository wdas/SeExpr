
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



## project name & version
PROJECT( SeExpr )
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

## policies     
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

# macros
include(src/build/macros.cmake)

## Setup platform specific helper defines build variants
IF(WIN32)
  include (GenerateExportHeader)
  ADD_DEFINITIONS (-DSEEXPR_WIN32)
ELSE(WIN32)
  ADD_DEFINITIONS (-Wall  -Wextra)
  ADD_DEFINITIONS (-pthread)

  SET( CMAKE_CXX_FLAGS "-fPIC")
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
SET( SEEXPR_LIBRARIES SeExpr )
SET( SEEXPR_EDITOR_LIBRARIES SeExprEditor )
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
ADD_SUBDIRECTORY (src/doc)
ADD_SUBDIRECTORY (src/demos)
ADD_SUBDIRECTORY (src/tests)



