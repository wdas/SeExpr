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

## project name & version
PROJECT (SeExpr2)
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
## allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
## Use verbose make file
SET ( CMAKE_VERBOSE_MAKEFILE TRUE )

## policies
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# macros
include(GNUInstallDirs)
include(src/build/macros.cmake)

# Default installation prefix
IF(NOT DEFINED FLAVOR)
    SET(FLAVOR "optimize" CACHE STRING "Build flavor")
ENDIF()

## Choose build options
# Disney specific method of choosing variant
IF(${FLAVOR} STREQUAL "optimize")
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "type of build" FORCE)
ENDIF(${FLAVOR} STREQUAL "optimize")

IF(${FLAVOR} STREQUAL "debug")
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "type of build" FORCE)
ENDIF(${FLAVOR} STREQUAL "debug")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    EXECUTE_PROCESS(
        COMMAND sh -c "echo `uname -s`-`uname -r | cut -d- -f1`-`uname -m`"
        OUTPUT_VARIABLE VARIANT_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
    SET(CMAKE_INSTALL_PREFIX
        "${CMAKE_SOURCE_DIR}/${VARIANT_DIRECTORY}-${FLAVOR}"
        CACHE PATH "Installation prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")

# Configuration options
# TODO: find GTEST and boost and python the proper ways
SET (BOOST_DIR /usr CACHE PATH "where boost is located")
SET (BOOST_PYTHON_LIBNAME boost_python CACHE STRING "what library name for boost python")
SET (GTEST_DIR /usr CACHE PATH "Where to find GTEST") # /usr/include/gtest)
SET (ENABLE_LLVM_BACKEND ON CACHE BOOL "Whether to build with LLVM backend")
SET (USE_PYTHON ON CACHE BOOL "Whether to compile python libraries")

if (ENABLE_LLVM_BACKEND)
    SET (LLVM_DIR /usr/share/llvm/cmake CACHE PATH "Where to search for LLVM i.e. ")

    FIND_PACKAGE(LLVM CONFIG NAMES LLVM CONFIGS LLVM-Config.cmake)
    if(LLVM_FOUND)
        set(SEEXPR_ENABLE_LLVM_BACKEND 1)
        message(STATUS "Using LLVM-Config.cmake in: ${LLVM_DIR}")
        find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config-3.8 llvm-config)

        # Uncomment to use clang++
        #set(CMAKE_CXX_COMPILER clang++)
        #set(CMAKE_C_COMPILER clang)

        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
        include(LLVM-Config)
        include(HandleLLVMOptions)

        message(STATUS "LLVM_DEFINITIONS =" ${LLVM_DEFINITIONS})
        add_definitions(${LLVM_DEFINITIONS})

        execute_process(
            COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
            OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(STATUS "LLVM_INCLUDE_DIR =" ${LLVM_INCLUDE_DIR})
        include_directories(${LLVM_INCLUDE_DIR})

        execute_process(
            COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
            OUTPUT_VARIABLE LLVM_LIBRARY_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(STATUS "LLVM_LIBRARY_DIR =" ${LLVM_LIBRARY_DIR})
        link_directories(${LLVM_LIBRARY_DIR})

    else(LLVM_FOUND)
         set(ENABLE_LLVM_BACKEND off)
         set(SEEXPR_ENABLE_LLVM_BACKEND 0)
    endif(LLVM_FOUND)
endif(ENABLE_LLVM_BACKEND)

## Setup platform specific helper defines build variants
IF(WIN32)
  include (GenerateExportHeader)
  ADD_DEFINITIONS (-DSEEXPR_WIN32)
ELSE(WIN32)
  ADD_DEFINITIONS (-Wall -Wextra -Wno-unused-parameter)
  ADD_DEFINITIONS (-pthread)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -std=c++11 -msse4.1")
  if(NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-date-time")
  endif()
ENDIF(WIN32)

# Set to release if nothing else defined
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(DEFINED ANIMLIB_DIR)
    MESSAGE(STATUS "ANIMLIB_DIR = ${ANIMLIB_DIR}")
    ADD_DEFINITIONS (-DSEEXPR_USE_ANIMLIB)
    INCLUDE_DIRECTORIES(${ANIMLIB_DIR}/include)
    LINK_DIRECTORIES(${ANIMLIB_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF(DEFINED ANIMLIB_DIR)

IF(DEFINED DERR_DIR)
    MESSAGE(STATUS "DERR_DIR = ${DMSG_DIR}")
    INCLUDE_DIRECTORIES(${DERR_DIR}/include)
    LINK_DIRECTORIES(${DERR_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF(DEFINED DERR_DIR)

IF(DEFINED DMSG_DIR)
    MESSAGE(STATUS "DMSG_DIR = ${DMSG_DIR}")
    INCLUDE_DIRECTORIES(${DMSG_DIR}/include)
    LINK_DIRECTORIES(${DMSG_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF(DEFINED DMSG_DIR)

IF(DEFINED QDGUI_DIR)
    MESSAGE(STATUS "QDGUI_DIR = ${QDGUI_DIR}")
    ADD_DEFINITIONS (-DSEEXPR_USE_QDGUI)
    INCLUDE_DIRECTORIES(${QDGUI_DIR}/include)
    LINK_DIRECTORIES(${QDGUI_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF(DEFINED QDGUI_DIR)

## Install location for html documentation
SET( SEEXPR_HTML_DOC_PATH "share/doc/SeExpr" )

## Make modules able to see seexpr library
# Setup environment variable to link seexpr
SET( SEEXPR_LIBRARIES SeExpr2 )
SET( SEEXPR_LLVM_LIBRARIES SeExpr2LLVM )
SET( SEEXPR_EDITOR_LIBRARIES SeExpr2Editor )

# make it so seexpr can be found
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_BINARY_DIR}/include )
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_BINARY_DIR}/src/SeExpr )
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src/SeExpr )
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src/EditorUI )


set(LLVM_LIB "")
IF(ENABLE_LLVM_BACKEND AND LLVM_FOUND)
    #todo infinite loop in this?
    #llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

    # construct library name
    execute_process(
        COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
        OUTPUT_VARIABLE LLVM_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    #set(LLVM_LIB "-lLLVM-${LLVM_VERSION}")
    link_directories(${LLVM_LIBRARY_DIRS})
    #set(LLVM_LIB ${LLVM_AVAILABLE_LIBS} -lLLVMSupport -lLLVMCodeGen -lLLVMCore) #"-lLLVMCore -lLLVMMCJIT -lLLVMExecutionEngine -lLL")
    set(LLVM_LIB LLVM)
    message(STATUS "LLVM library = " ${LLVM_LIB})
    message(STATUS "LLVM version = " ${LLVM_VERSION})
    if(LLVM_VERSION VERSION_LESS 3.8.0)
        set(ENABLE_LLVM_BACKEND off)
        set(SEEXPR_ENABLE_LLVM_BACKEND 0)
        message(STATUS "Not building with LLVM, version must be >= 3.8.0")
    endif(LLVM_VERSION VERSION_LESS 3.8.0)
ENDIF(ENABLE_LLVM_BACKEND AND LLVM_FOUND)

## Traverse subdirectories
ADD_SUBDIRECTORY (src/SeExpr)
ADD_SUBDIRECTORY (src/ui)
ADD_SUBDIRECTORY (src/tests)
ADD_SUBDIRECTORY (src/py)
ADD_SUBDIRECTORY (src/utils)
ADD_SUBDIRECTORY (src/demos)
ADD_SUBDIRECTORY (src/doc)
