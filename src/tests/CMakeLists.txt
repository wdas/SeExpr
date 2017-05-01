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

if(EXISTS ${GTEST_DIR}/include)
    # Google test framework
    include_directories(BEFORE ${GTEST_DIR}/include)
    include_directories(${CMAKE_BINARY_DIR}/src/SeExpr)
    link_directories(${GTEST_DIR}/${CMAKE_INSTALL_LIBDIR})

    # Uncomment below to print performance stats
    # add_definitions(-DSEEXPR_PERFORMANCE)

    if (DEFINED PNG_DIR)
        message(STATUS "PNG_DIR = ${PNG_DIR}")
        include_directories(${PNG_DIR}/include)
        find_library(PNG_LIBRARIES png PATHS ${PNG_DIR}/${CMAKE_INSTALL_LIBDIR})
        set(PNG_FOUND 1)
    endif()

    if (PNG_FOUND)
        message("-- Found PNG library: " ${PNG_LIBRARIES})

        include_directories(SYSTEM ${PNG_INCLUDE_DIR})
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})

        # generate test cases for example developer expressions
        set (GENERATE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py")
        set (EXAMPLE_DIR ${CMAKE_INSTALL_PREFIX}/share/SeExpr2/demos/imageSynth/examples)
        if (EXISTS ${EXAMPLE_DIR})
            set (EXAMPLE_TESTS "testSeExprExamples.cpp")
            add_custom_command(
                SOURCE ${EXAMPLE_DIR}
                COMMAND ${GENERATE_SCRIPT}
                ARGS "${EXAMPLE_DIR}" "${EXAMPLE_TESTS}"
                OUTPUT ${EXAMPLE_TESTS}
                DEPENDS ${GENERATE_SCRIPT} ${EXAMPLE_DIR})
        endif()

        # generate test cases for paint3d show examples
        set (PAINT3D_DIR /disney/shows/default/rel/global/expressions)
        if (EXISTS ${PAINT3D_DIR})
            set (PAINT3D_TESTS "testPaint3dExamples.cpp")
            add_custom_command(
                SOURCE ${PAINT3D_DIR}
                COMMAND ${GENERATE_SCRIPT}
                ARGS "${PAINT3D_DIR}" "${PAINT3D_TESTS}"
                OUTPUT ${PAINT3D_TESTS}
                DEPENDS ${GENERATE_SCRIPT} ${PAINT3D_DIR})
        endif()

        add_executable(testmain2 "testmain.cpp" "imageTests.cpp" ${EXAMPLE_TESTS} ${PAINT3D_TESTS} "basic.cpp" "string.cpp")
        add_dependencies(testmain2 imageSynth2)
        target_link_libraries(testmain2 SeExpr2 gtest ${PNG_LIBRARIES})
        install(TARGETS testmain2 DESTINATION share/test/SeExpr2)
        install(PROGRAMS imagediff.py DESTINATION share/test/SeExpr2)

        add_test(NAME basic COMMAND testmain2 --gtest_filter="BasicTests.*")
    else()
        message(STATUS "Couldn't find PNG -- not doing tests")
    endif()
else()
    message(STATUS "Couldn't find gtest framework -- not building main tests")
endif()

add_executable(dirtSimple "dirtSimple.cpp")
target_link_libraries(dirtSimple SeExpr2 ${PNG_LIBRARIES})
install(TARGETS dirtSimple DESTINATION bin)

add_executable(BlockTests "BlockTests.cpp")
target_link_libraries(BlockTests SeExpr2 ${PNG_LIBRARIES})
install(TARGETS BlockTests DESTINATION bin)

add_executable(VarBlockExample VarBlockExample.cpp)
target_link_libraries(VarBlockExample SeExpr2)
install(TARGETS VarBlockExample DESTINATION bin)
