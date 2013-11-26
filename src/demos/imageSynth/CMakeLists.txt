
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

FIND_PACKAGE(PNG)
IF(PNG_FOUND)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
    ADD_EXECUTABLE(imageSynth "imageSynth.cpp")
    TARGET_LINK_LIBRARIES(imageSynth ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynth DESTINATION bin)
ENDIF(PNG_FOUND)
