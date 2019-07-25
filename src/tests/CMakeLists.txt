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

include_directories(${CMAKE_BINARY_DIR}/src/SeExpr2)

set(TEST_DEST "share/test/SeExpr2")

if (GTEST_DIR)
    # Google test framework
    include_directories(AFTER ${GTEST_DIR}/include)
    find_library(GTEST_LIBRARIES gtest PATHS ${GTEST_DIR}/${CMAKE_INSTALL_LIBDIR})

    # Uncomment below to print performance stats
    # add_definitions(-DSEEXPR_PERFORMANCE)

    add_executable(test-basic testmain.cpp basic.cpp)
    target_link_libraries(test-basic SeExpr2 ${GTEST_LIBRARIES})
    install(TARGETS test-basic DESTINATION ${TEST_DEST})
    add_test(NAME basic COMMAND test-basic)

    add_executable(test-string testmain.cpp string.cpp)
    target_link_libraries(test-string SeExpr2 ${GTEST_LIBRARIES})
    install(TARGETS test-string DESTINATION ${TEST_DEST})
    add_test(NAME string COMMAND test-string)

    if (DEFINED PNG_DIR)
        message(STATUS "PNG_DIR = ${PNG_DIR}")
        include_directories(${PNG_DIR}/include)
        find_library(PNG_LIBRARIES png libpng PATHS ${PNG_DIR}/${CMAKE_INSTALL_LIBDIR})
        set(PNG_FOUND 1)
    else()
        find_package(PNG QUIET)
    endif()

    if (PNG_FOUND)
        message("-- Found PNG library: " ${PNG_LIBRARIES})
        include_directories(SYSTEM ${PNG_INCLUDE_DIR})
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})

        # generate test cases for example developer expressions
        set (GENERATE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py")
        set (EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/src/demos/imageSynth/examples)
        if (EXISTS ${EXAMPLE_DIR})
            set(EXAMPLE_TESTS testSeExprExamples.cpp)
            add_custom_command(
                SOURCE ${EXAMPLE_DIR}
                COMMAND ${GENERATE_SCRIPT}
                ARGS "${EXAMPLE_DIR}" "${EXAMPLE_TESTS}"
                OUTPUT ${EXAMPLE_TESTS}
                DEPENDS ${GENERATE_SCRIPT} ${EXAMPLE_DIR})

            add_executable(test-examples testmain.cpp imageTests.cpp ${EXAMPLE_TESTS})
            target_link_libraries(test-examples SeExpr2 ${GTEST_LIBRARIES} ${PNG_LIBRARIES})

            install(TARGETS test-examples DESTINATION ${TEST_DEST})
            add_test(NAME examples COMMAND test-examples)
        endif()

        # generate test cases for paint3d show examples
        set (PAINT3D_DIR /disney/shows/default/rel/global/expressions)
        if (EXISTS ${PAINT3D_DIR})
            set(PAINT3D_TESTS  testPaint3dExamples.cpp)
            add_custom_command(
                SOURCE ${PAINT3D_DIR}
                COMMAND ${GENERATE_SCRIPT}
                ARGS "${PAINT3D_DIR}" "${PAINT3D_TESTS}"
                OUTPUT ${PAINT3D_TESTS}
                DEPENDS ${GENERATE_SCRIPT} ${PAINT3D_DIR})
            add_executable(test-p3d-examples testmain.cpp ${PAINT3D_TESTS})
            target_link_libraries(test-p3d-examples SeExpr2 ${GTEST_LIBRARIES})
            install(TARGETS test-p3d-examples DESTINATION ${TEST_DEST})
            add_test(NAME p3d-examples COMMAND test-p3d-examples)
        endif()

        add_executable(test-images testmain.cpp imageTests.cpp)
        target_link_libraries(test-images SeExpr2 ${GTEST_LIBRARIES} ${PNG_LIBRARIES})
        install(TARGETS test-images DESTINATION ${TEST_DEST})
        add_test(NAME images COMMAND test-images)

        install(PROGRAMS imagediff.py DESTINATION ${TEST_DEST})
    else()
        message(STATUS "Couldn't find PNG -- not image doing tests")
    endif()
else()
    message(STATUS "Couldn't find gtest framework -- not building main tests")
endif()

add_test(NAME basicPython
         COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/basicPython.py)

add_executable(test-simple dirtSimple.cpp)
target_link_libraries(test-simple SeExpr2)
install(TARGETS test-simple DESTINATION ${TEST_DEST})
add_test(NAME simple COMMAND test-simple)

add_executable(BlockTests "BlockTests.cpp")
target_link_libraries(BlockTests SeExpr2 ${PNG_LIBRARIES})
install(TARGETS BlockTests DESTINATION ${TEST_DEST})
add_test(NAME BlockTests COMMAND BlockTests)

add_executable(VarBlockExample VarBlockExample.cpp)
target_link_libraries(VarBlockExample SeExpr2)
install(TARGETS VarBlockExample DESTINATION ${TEST_DEST})

if (ENABLE_SLOW_TESTS)
    add_test(NAME imageTestsReport
             COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/imageTestsReportNew.py runall)

    add_test(NAME VarBlockExample COMMAND VarBlockExample)
endif()
