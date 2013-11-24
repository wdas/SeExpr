
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

ADD_EXECUTABLE(asciiGraph "asciiGraph.cpp")
target_link_libraries(asciiGraph ${SEEXPR_LIBRARIES})
install(TARGETS asciiGraph DESTINATION bin)

ADD_EXECUTABLE(asciiCalc "asciiCalculator.cpp")
target_link_libraries(asciiCalc ${SEEXPR_LIBRARIES})
install(TARGETS asciiCalc DESTINATION bin)



ADD_SUBDIRECTORY (imageSynth)
ADD_SUBDIRECTORY (imageEditor)
ADD_SUBDIRECTORY (segraph)
ADD_SUBDIRECTORY (rman)
