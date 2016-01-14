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

find_package(PNG)
if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIR})
    ADD_EXECUTABLE("testmain2" "testmain.cpp" "imageTests.cpp")
    ADD_EXECUTABLE("dirtSimple" "dirtSimple.cpp")
    target_link_libraries("dirtSimple"  ${SEEXPR_LIBRARIES} ${PNG_LIBRARIES})
    #ADD_EXECUTABLE("testmain2" "testmain.cpp" "imageTests.cpp" "basic.cpp" "typeTests.cpp" "multiTests.cpp")
    target_link_libraries("testmain2" gtest pcrecpp ${SEEXPR_LIBRARIES} ${PNG_LIBRARIES})
    install(TARGETS "testmain2" DESTINATION share/test/SeExpr2)
    install(TARGETS "dirtSimple" DESTINATION bin)
    install(PROGRAMS imagediff.py DESTINATION share/test/SeExpr2)
endif(PNG_FOUND)
