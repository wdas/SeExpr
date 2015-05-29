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
set (GTEST_INCLUDE_DIR /usr/include/gtest /usr/include/gmock)
include_directories(BEFORE ${GTEST_INCLUDE_DIR})

# testmain running test suite using google test framework
ADD_EXECUTABLE("testmain" "testmain.cpp" "basic.cpp" "typeTests.cpp"
"multiTests.cpp")
target_link_libraries("testmain" gtest gmock ${SEEXPR_LIBRARIES} ${SEEXPR_EDITOR_LIBRARIES})
install(TARGETS "testmain" DESTINATION test)


#set(TOOLS listVar patterns TypePrinter eval)

#foreach(item ${TOOLS})
    #ADD_EXECUTABLE("${item}2" "${item}.cpp")
    #target_link_libraries("${item}2" gtest ${SEEXPR_LIBRARIES} ${SEEXPR_EDITOR_LIBRARIES})
    #install(TARGETS "${item}2" DESTINATION test)
#endforeach(item)

