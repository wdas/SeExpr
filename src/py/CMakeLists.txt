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

IF(USE_PYTHON)

IF(APPLE)
#FIND_PACKAGE(PythonLibs)
#    if(${CUSTOM_PYTHON_FRAMEWORK})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_PYTHON_FRAMEWORK}")
        set(CMAKE_LINK_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_PYTHON_FRAMEWORK}")
#    endif()

ELSE(APPLE)

IF (NOT DEFINED PYTHON_VERSION)
    EXECUTE_PROCESS(
        COMMAND
        python -c "import sys; print('%s.%s' % sys.version_info[:2])"
        OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF(NOT DEFINED PYTHON_VERSION)
MESSAGE(STATUS "PYTHON_VERSION = ${PYTHON_VERSION}")

IF (NOT DEFINED PYTHON_INCLUDE_DIR)
    EXECUTE_PROCESS(
        COMMAND
        sh -c "python-config --includes | sed -e s,-I,,g"
        OUTPUT_VARIABLE PYTHON_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(PYTHON_INCLUDE_DIR)
ENDIF(NOT DEFINED PYTHON_INCLUDE_DIR)
MESSAGE(STATUS "PYTHON_INCLUDE_DIR = ${PYTHON_INCLUDE_DIR}")
include_directories(${PYTHON_INCLUDE_DIR})

IF (NOT DEFINED PYTHON_LIB_DIRS)
    EXECUTE_PROCESS(
        COMMAND
        sh -c "python-config --ldflags | tr ' ' '\\n' | grep ^-L | sed -e s,-L,, | tr '\\n' ' '"
        OUTPUT_VARIABLE PYTHON_LIB_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
    IF (NOT ${PYTHON_LIB_DIRS} STREQUAL "")
        separate_arguments(PYTHON_LIB_DIRS)
    ENDIF(NOT ${PYTHON_LIB_DIRS} STREQUAL "")
ENDIF(NOT DEFINED PYTHON_LIB_DIRS)

IF (DEFINED PYTHON_LIB_DIRS)
    MESSAGE(STATUS "PYTHON_LIB_DIRS = ${PYTHON_LIB_DIRS}")
    link_directories(${PYTHON_LIB_DIRS})
ENDIF (DEFINED PYTHON_LIB_DIRS)

IF (NOT DEFINED PYTHON_LIBRARIES)
    EXECUTE_PROCESS(
        COMMAND
        sh -c "python-config --ldflags | tr ' ' '\\n' | grep ^-l | sed -e s,-l,, | tr '\\n' ' '"
        OUTPUT_VARIABLE PYTHON_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(PYTHON_LIBRARIES)
ENDIF(NOT DEFINED PYTHON_LIBRARIES)
MESSAGE(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")

ENDIF(APPLE)

SET(PYTHON_DEST "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages/SeExprPy" )
message(STATUS "PYTHON_DEST = ${PYTHON_DEST}")

IF(NOT DEFINED BOOST_INCLUDE_DIR)
    SET(BOOST_INCLUDE_DIR ${BOOST_DIR}/include)
ENDIF(NOT DEFINED BOOST_INCLUDE_DIR)
include_directories(SYSTEM ${BOOST_INCLUDE_DIR})

IF(NOT DEFINED BOOST_LIB_DIR)
    SET(BOOST_LIB_DIR ${BOOST_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF(NOT DEFINED BOOST_LIB_DIR)
link_directories(${BOOST_LIB_DIR})

include_directories(../SeExpr/parser)
add_library(core SHARED SeExprPy.cpp ../SeExpr/parser/SeExprParse.cpp ../SeExpr/parser/SeExprLex.cpp)

if(APPLE)
    # TODO: figure out how to get this to work
else(APPLE)
    target_link_libraries(core ${PYTHON_LIBRARIES})
endif(APPLE)
target_link_libraries(core ${BOOST_PYTHON_LIBNAME})

set_target_properties(core PROPERTIES PREFIX "")

INSTALL (TARGETS core DESTINATION ${PYTHON_DEST})
INSTALL (FILES utils.py DESTINATION ${PYTHON_DEST})
INSTALL (FILES __init__.py DESTINATION ${PYTHON_DEST})

ENDIF(USE_PYTHON)
