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

    ADD_EXECUTABLE(imageSynth2 "imageSynth.cpp")
    TARGET_LINK_LIBRARIES(imageSynth2 ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynth2 DESTINATION bin)

    ADD_EXECUTABLE(imageSynthPaint3d2 "imageSynthForPaint3d.cpp")
    TARGET_LINK_LIBRARIES(imageSynthPaint3d2 ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
    install(TARGETS imageSynthPaint3d2 DESTINATION bin)

    if (ENABLE_LLVM_BACKEND)
        ADD_EXECUTABLE(imageSynthLLVMPaint3d "imageSynthForPaint3d.cpp")
        set_target_properties(imageSynthLLVMPaint3d PROPERTIES COMPILE_FLAGS "-DSEEXPR_ENABLE_LLVM")
        TARGET_LINK_LIBRARIES(imageSynthLLVMPaint3d ${PNG_LIBRARIES} ${SEEXPR_LIBRARIES})
        install(TARGETS imageSynthLLVMPaint3d DESTINATION bin)
    endif()

    install(DIRECTORY examples DESTINATION share/SeExpr2/demos/imageSynth USE_SOURCE_PERMISSIONS)
ENDIF(PNG_FOUND)
