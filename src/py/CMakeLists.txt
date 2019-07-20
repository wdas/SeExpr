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

if (USE_PYTHON)

    if (APPLE)
        #find_package(PythonLibs)
        #if (${CUSTOM_PYTHON_FRAMEWORK})
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_PYTHON_FRAMEWORK}")
            set(CMAKE_LINK_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_PYTHON_FRAMEWORK}")
        #endif()
    else()
        IF (NOT DEFINED PYTHON_VERSION)
            execute_process(
                COMMAND
                python -c "import sys; print('%s.%s' % sys.version_info[:2])"
                OUTPUT_VARIABLE PYTHON_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
        message(STATUS "PYTHON_VERSION = ${PYTHON_VERSION}")

        if (NOT DEFINED PYTHON_INCLUDE_DIR)
            execute_process(
                COMMAND
                sh -c "python-config --includes | sed -e s,-I,,g"
                OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
            separate_arguments(PYTHON_INCLUDE_DIR)
        endif()
        message(STATUS "PYTHON_INCLUDE_DIR = ${PYTHON_INCLUDE_DIR}")
        include_directories(${PYTHON_INCLUDE_DIR})

        if (NOT DEFINED PYTHON_LIB_DIRS)
            execute_process(
                COMMAND
                sh -c "python-config --ldflags | tr ' ' '\\n' | grep ^-L | sed -e s,-L,, | tr '\\n' ' '"
                OUTPUT_VARIABLE PYTHON_LIB_DIRS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
            if (NOT ${PYTHON_LIB_DIRS} STREQUAL "")
                separate_arguments(PYTHON_LIB_DIRS)
            endif()
        endif()

        if (DEFINED PYTHON_LIB_DIRS)
            message(STATUS "PYTHON_LIB_DIRS = ${PYTHON_LIB_DIRS}")
            link_directories(${PYTHON_LIB_DIRS})
        endif()

        if (NOT DEFINED PYTHON_LIBRARIES)
            execute_process(
                COMMAND
                sh -c "python-config --ldflags | tr ' ' '\\n' | grep ^-l | sed -e s,-l,, | tr '\\n' ' '"
                OUTPUT_VARIABLE PYTHON_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
            separate_arguments(PYTHON_LIBRARIES)
        endif()
        message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")

    endif()

    set(PYTHON_DEST "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages/SeExprPy")
    message(STATUS "PYTHON_DEST = ${PYTHON_DEST}")

    if (NOT DEFINED BOOST_INCLUDE_DIR)
        set(BOOST_INCLUDE_DIR ${BOOST_DIR}/include)
    endif()
    include_directories(SYSTEM ${BOOST_INCLUDE_DIR} ${PYTHON_INCLUDE_DIR})
    message(STATUS "BOOST_INCLUDE_DIR = ${BOOST_INCLUDE_DIR}")

    if (NOT DEFINED BOOST_LIB_DIR)
        set(BOOST_LIB_DIR ${BOOST_DIR}/${CMAKE_INSTALL_LIBDIR})
    endif()
    link_directories(${BOOST_LIB_DIR})
    message(STATUS "BOOST_LIB_DIR = ${BOOST_LIB_DIR}")

    include_directories(../SeExpr2/parser)
    add_library(core SHARED SeExprPy.cpp ../SeExpr2/parser/SeExprParse.cpp ../SeExpr2/parser/SeExprLex.cpp)

    if (APPLE)
        # TODO: figure out how to get this to work
    else()
        target_link_libraries(core ${PYTHON_LIBRARIES})
    endif()
    target_link_libraries(core ${BOOST_PYTHON_LIBNAME})
    set_target_properties(core PROPERTIES PREFIX "")

    install(TARGETS core DESTINATION ${PYTHON_DEST})
    install(FILES utils.py DESTINATION ${PYTHON_DEST})
    install(FILES __init__.py DESTINATION ${PYTHON_DEST})
endif()
