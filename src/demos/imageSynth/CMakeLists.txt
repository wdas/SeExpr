# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

FIND_PACKAGE(PNG)
IF(PNG_FOUND)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
    ADD_EXECUTABLE(imageSynth "imageSynth.cpp")
    TARGET_LINK_LIBRARIES(imageSynth ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynth DESTINATION bin)
ENDIF(PNG_FOUND)
