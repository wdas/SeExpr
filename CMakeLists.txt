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
cmake_minimum_required(VERSION 2.4.6)

## project name & version
project(SeExpr2)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_LEVEL 0)

enable_testing()

mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)

## allow more human readable "if then else" constructs
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

## Use verbose make file
set(CMAKE_VERBOSE_MAKEFILE true)

## policies
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif()

# macros
include(GNUInstallDirs)
include(src/build/macros.cmake)

# Default installation prefix
if (NOT DEFINED FLAVOR)
    set(FLAVOR "optimize" CACHE STRING "Build flavor")
endif()

## Choose build options
# Disney specific method of choosing variant
if (${FLAVOR} STREQUAL "optimize")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "type of build" FORCE)
endif()

if (${FLAVOR} STREQUAL "debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "type of build" FORCE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    execute_process(
        COMMAND sh -c "echo `uname -s`-`uname -r | cut -d- -f1`-`uname -m`"
        OUTPUT_VARIABLE VARIANT_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_SOURCE_DIR}/${VARIANT_DIRECTORY}-${FLAVOR}"
        CACHE PATH "Installation prefix" FORCE)
endif()

message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")

# Configuration options
# TODO: find GTEST and boost and python the proper ways
set(BOOST_DIR /usr CACHE PATH "where boost is located")
find_package(PythonInterp) # provides Python version check
if(PYTHON_VERSION_MAJOR LESS 3)
    set(BOOST_PYTHON_LIBNAME boost_python CACHE STRING "what library name for boost python")
else()
    # try to find boost_python3 in a way compatible with most distributions
    find_library(BOOST_PYTHON3_LIB NAMES "boost_python3" "boost_python3${PYTHON_VERSION_MINOR}")
    SET (BOOST_PYTHON_LIBNAME ${BOOST_PYTHON3_LIB} CACHE STRING "what library name for boost python")
endif()
set(GTEST_DIR /usr CACHE PATH "Where to find GTEST") # /usr/include/gtest)
set(ENABLE_LLVM_BACKEND ON CACHE BOOL "Whether to build with LLVM backend")
set(USE_PYTHON ON CACHE BOOL "Whether to compile python libraries")

set(LLVM_LIB "")
if (ENABLE_LLVM_BACKEND)
    set(LLVM_DIR /usr/share/llvm/cmake CACHE PATH "Where to search for LLVM i.e. ")

    find_package(LLVM CONFIG NAMES LLVM CONFIGS LLVM-Config.cmake LLVMConfig.cmake)
    if (LLVM_FOUND)
        set(SEEXPR_ENABLE_LLVM_BACKEND 1)
        message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
        find_program(LLVM_CONFIG_EXECUTABLE NAMES ${LLVM_TOOLS_BINARY_DIR}/llvm-config)

        # Uncomment to use clang++
        #set(CMAKE_CXX_COMPILER clang++)
        #set(CMAKE_C_COMPILER clang)

        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
        include(LLVM-Config)
        include(HandleLLVMOptions)

        message(STATUS "LLVM_DEFINITIONS =" ${LLVM_DEFINITIONS})
        add_definitions(${LLVM_DEFINITIONS})

        if (NOT LLVM_CONFIG_EXECUTABLE STREQUAL "LLVM_CONFIG_EXECUTABLE-NOTFOUND")
            execute_process(
                COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
                OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        else ()
            set(LLVM_INCLUDE_DIR ${LLVM_INCLUDE_DIRS})
        endif ()
        message(STATUS "LLVM_INCLUDE_DIR =" ${LLVM_INCLUDE_DIR})
        include_directories(${LLVM_INCLUDE_DIR})

        if (NOT LLVM_CONFIG_EXECUTABLE STREQUAL "LLVM_CONFIG_EXECUTABLE-NOTFOUND")
            execute_process(
                COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
                OUTPUT_VARIABLE LLVM_LIBRARY_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        else ()
            set(LLVM_LIBRARY_DIR ${LLVM_LIBRARY_DIRS})
        endif ()
        message(STATUS "LLVM_LIBRARY_DIR =" ${LLVM_LIBRARY_DIR})
        link_directories(${LLVM_LIBRARY_DIR})

        #todo infinite loop in this?
        #llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

        # construct library name
        if (NOT LLVM_CONFIG_EXECUTABLE STREQUAL "LLVM_CONFIG_EXECUTABLE-NOTFOUND")
            execute_process(
                COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
                OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        else ()
            set(LLVM_VERSION ${LLVM_PACKAGE_VERSION})
        endif ()
        message(STATUS "LLVM_VERSION = ${LLVM_VERSION}")

        set(LLVM_LIB LLVM)
        message(STATUS "LLVM_LIB = ${LLVM_LIB}")

        if (LLVM_VERSION VERSION_LESS 3.8.0)
            set(ENABLE_LLVM_BACKEND false)
            set(SEEXPR_ENABLE_LLVM_BACKEND false)
            message(STATUS "Not building with LLVM, version must be >= 3.8.0")
        endif()

    else()
         set(ENABLE_LLVM_BACKEND off)
         set(SEEXPR_ENABLE_LLVM_BACKEND 0)
    endif()
endif()

## Setup platform specific helper defines build variants
if (WIN32)
    include (GenerateExportHeader)
    add_definitions(-DSEEXPR_WIN32)
else()
    add_definitions(-Wall -Wextra -Wno-unused-parameter)
    add_definitions(-pthread)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -std=c++11 -msse4.1")
    if (NOT APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-date-time")
    endif()
endif()

# Set to release if nothing else defined
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

if (ENABLE_QT5)
    find_package(Qt5 COMPONENTS Core Gui OpenGL)
    if (Qt5_FOUND)
        message(STATUS "Qt5 Enabled")
        message(STATUS "Qt5Core_INCLUDE_DIRS = ${Qt5Core_INCLUDE_DIRS}")
        message(STATUS "Qt5Gui_INCLUDE_DIRS = ${Qt5Gui_INCLUDE_DIRS}")
        message(STATUS "Qt5OpenGL_INCLUDE_DIRS = ${Qt5OpenGL_INCLUDE_DIRS}")
        include_directories(${Qt5Core_INCLUDE_DIRS})
        include_directories(${Qt5Gui_INCLUDE_DIRS})
        include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    endif()
else()
    find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL)
    if (QT4_FOUND)
        message(STATUS "Qt4 Enabled")
        message(STATUS "QT_INCLUDE_DIR = ${QT_INCLUDE_DIR}")
        include_directories(${QT_INCLUDE_DIR}
                            ${QT_INCLUDE_DIR}/QtCore
                            ${QT_INCLUDE_DIR}/QtGui
                            ${QT_INCLUDE_DIR}/QtOpenGL)
    endif()
endif()

if (DEFINED ANIMLIB_DIR)
    message(STATUS "ANIMLIB_DIR = ${ANIMLIB_DIR}")
    add_definitions(-DSEEXPR_USE_ANIMLIB)
    include_directories(${ANIMLIB_DIR}/include)
    link_directories(${ANIMLIB_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if (DEFINED DERR_DIR)
    message(STATUS "DERR_DIR = ${DMSG_DIR}")
    include_directories(${DERR_DIR}/include)
    link_directories(${DERR_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if (DEFINED DMSG_DIR)
    message(STATUS "DMSG_DIR = ${DMSG_DIR}")
    include_directories(${DMSG_DIR}/include)
    link_directories(${DMSG_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if (DEFINED QDGUI_DIR)
    message(STATUS "QDGUI_DIR = ${QDGUI_DIR}")
    add_definitions(-DSEEXPR_USE_QDGUI)
    include_directories(${QDGUI_DIR}/include)
    link_directories(${QDGUI_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

## Install location for html documentation
set(SEEXPR_HTML_DOC_PATH "share/doc/SeExpr2")

## Make modules able to see seexpr library
# Setup environment variable to link seexpr
set(SEEXPR_LIBRARIES SeExpr2)
set(SEEXPR_LLVM_LIBRARIES SeExpr2LLVM)
set(SEEXPR_EDITOR_LIBRARIES SeExpr2Editor)

# make it so seexpr can be found
include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)
include_directories(BEFORE ${CMAKE_BINARY_DIR}/src/SeExpr)
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/SeExpr)
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src/ui)

## Traverse subdirectories
add_subdirectory(src/SeExpr)
add_subdirectory(src/ui)
add_subdirectory(src/py)
add_subdirectory(src/utils)
add_subdirectory(src/demos)
add_subdirectory(src/doc)
add_subdirectory(src/tests)
