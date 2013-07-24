# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

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
