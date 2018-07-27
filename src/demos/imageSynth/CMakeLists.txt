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

if (NOT PNG_DIR)
    find_package(PNG QUIET)
endif()
if (PNG_FOUND)
    include_directories(SYSTEM ${PNG_INCLUDE_DIR})

    add_executable(imageSynth2 "imageSynth.cpp")
    target_link_libraries(imageSynth2 ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynth2 DESTINATION bin)

    add_executable(imageSynthPaint3d2 "imageSynthForPaint3d.cpp")
    target_link_libraries(imageSynthPaint3d2 ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynthPaint3d2 DESTINATION bin)
    install(DIRECTORY examples DESTINATION share/SeExpr2/demos/imageSynth
            USE_SOURCE_PERMISSIONS)
endif()
