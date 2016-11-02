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

include_directories(${CMAKE_BINARY_DIR}/src/SeExpr)

add_executable(asciiGraph2 "asciiGraph.cpp")
target_link_libraries(asciiGraph2 SeExpr2)
install(TARGETS asciiGraph2 DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(asciiCalc2 "asciiCalculator.cpp")
target_link_libraries(asciiCalc2 SeExpr2)
install(TARGETS asciiCalc2 DESTINATION ${CMAKE_INSTALL_BINDIR})

# TODO re-enable
add_subdirectory(imageSynth)
add_subdirectory(imageEditor)
#add_subdirectory(segraph)
#add_subdirectory(rman)

#if (ENABLE_LLVM_BACKEND)
#     add_executable(llvmtest "llvmtest.cpp")
#     target_link_libraries(llvmtest ${SEEXPR_LIBRARIES})
#     install(TARGETS llvmtest DESTINATION ${CMAKE_INSTALL_BINDIR})
#endif()
