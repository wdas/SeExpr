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

ADD_EXECUTABLE(asciiGraph2 "asciiGraph.cpp")
target_link_libraries(asciiGraph2 ${SEEXPR_LIBRARIES})
install(TARGETS asciiGraph2 DESTINATION bin)

ADD_EXECUTABLE(asciiCalc2 "asciiCalculator.cpp")
target_link_libraries(asciiCalc2 ${SEEXPR_LIBRARIES})
install(TARGETS asciiCalc2 DESTINATION bin)
 
#todo re-enable
ADD_SUBDIRECTORY (imageSynth2)
#ADD_SUBDIRECTORY (segraph)
#ADD_SUBDIRECTORY (rman)

# if(ENABLE_LLVM_BACKEND)
  # ADD_EXECUTABLE(llvmtest "llvmtest.cpp")
  # target_link_libraries(llvmtest ${SEEXPR_LIBRARIES})
  # install(TARGETS llvmtest DESTINATION bin)
# endif()
