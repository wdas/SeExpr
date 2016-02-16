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

# Google test framework
set (GTEST_INCLUDE_DIR /usr/include/gtest)
include_directories(BEFORE ${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/src/SeExpr)

# Uncomment below to print performance stats
# add_definitions(-DSEEXPR_PERFORMANCE)

find_package(PNG)
if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    # generate test cases for dev example expressions
    ADD_CUSTOM_COMMAND(
      SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../demos/imageSynth/examples
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py"
      ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../demos/imageSynth/examples" "testSeExprExamples.cpp"
      OUTPUT testSeExprExamples.cpp
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py
              ${CMAKE_CURRENT_SOURCE_DIR}/../demos/imageSynth/examples
    )
    # generate test cases for paint3d show examples
    ADD_CUSTOM_COMMAND(
      SOURCE /disney/shows/default/rel/global/expressions
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py"
      ARGS "/disney/shows/default/rel/global/expressions" "testPaint3dExamples.cpp"
      OUTPUT testPaint3dExamples.cpp
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py
              /disney/shows/default/rel/global/expressions
    )

    ADD_EXECUTABLE(testmain2 "testmain.cpp" "imageTests.cpp" "testSeExprExamples.cpp" "testPaint3dExamples.cpp")
    target_link_libraries(testmain2 SeExpr2 gtest pcrecpp ${PNG_LIBRARIES})
    ADD_EXECUTABLE(dirtSimple "dirtSimple.cpp")
    target_link_libraries(dirtSimple SeExpr2 ${PNG_LIBRARIES})
    if (ENABLE_LLVM_BACKEND)
        ADD_EXECUTABLE(testmain2LLVM "testmain.cpp" "imageTests.cpp" "testSeExprExamples.cpp" "testPaint3dExamples.cpp")
        set_target_properties(testmain2LLVM PROPERTIES COMPILE_FLAGS "-DSEEXPR_ENABLE_LLVM")
        target_link_libraries(testmain2LLVM SeExpr2LLVM gtest pcrecpp ${PNG_LIBRARIES})
        install(TARGETS testmain2LLVM DESTINATION share/test/SeExpr2)
    endif()
    install(TARGETS testmain2 DESTINATION share/test/SeExpr2)
    install(TARGETS dirtSimple DESTINATION bin)
    install(PROGRAMS imagediff.py DESTINATION share/test/SeExpr2)
endif(PNG_FOUND)
