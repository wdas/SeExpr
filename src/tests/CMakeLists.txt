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
    if(EXISTS /disney/shows/default/rel/global/expressions)
      set (PAINT3D_SRC  "testPaint3dExamples.cpp")
      ADD_CUSTOM_COMMAND(
      SOURCE /disney/shows/default/rel/global/expressions
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py"
      ARGS "/disney/shows/default/rel/global/expressions" "testPaint3dExamples.cpp"
      OUTPUT testPaint3dExamples.cpp
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/genImageTestFile.py
              /disney/shows/default/rel/global/expressions
    )
    endif()

    ADD_EXECUTABLE(testmain2 "testmain.cpp" "imageTests.cpp" "testSeExprExamples.cpp" ${PAINT3D_SRC} "basic.cpp")
    target_link_libraries(testmain2 SeExpr2 gtest ${PNG_LIBRARIES})
    install(TARGETS testmain2 DESTINATION share/test/SeExpr2)
    install(PROGRAMS imagediff.py DESTINATION share/test/SeExpr2)
else(PNG_FOUND)
    message("Couldn't find PNG not doing tests")
endif(PNG_FOUND)
else()
    message("Couldn't find gtest framework not building main tests")
endif()

ADD_EXECUTABLE(dirtSimple "dirtSimple.cpp")
target_link_libraries(dirtSimple SeExpr2 ${PNG_LIBRARIES})
install(TARGETS dirtSimple DESTINATION bin)

ADD_EXECUTABLE(BlockTests "BlockTests.cpp")
target_link_libraries(BlockTests SeExpr2 ${PNG_LIBRARIES})
install(TARGETS BlockTests DESTINATION bin)

ADD_EXECUTABLE(VarBlockExample VarBlockExample.cpp)
target_link_libraries(VarBlockExample SeExpr2)
install(TARGETS VarBlockExample DESTINATION bin)
