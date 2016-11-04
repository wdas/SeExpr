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

# Non optimal, can't figure out better way right now
set(RMANTREE_TEST $ENV{RMANTREE} CACHE STRING stuff FORCE)
string(COMPARE EQUAL "${RMANTREE_TEST}" "" RMAN_SET)

if (NOT ${RMAN_SET})
    include_directories($ENV{RMANTREE}/include)
    link_directories($ENV{RMANTREE}/lib)

    add_library(SeExprOp2 SHARED seop.cpp)
    target_link_libraries(SeExprOp2 prman ${SEEXPR_LIBRARIES})
    install(TARGETS SeExprOp2 DESTINATION prman)

    foreach (SHADER_BASENAME testdisp testse)
        set(SHADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE})
        add_custom_command(
            SOURCE ${SHADER_FILE}.sl
            COMMAND shader
            ARGS -o ${SHADER_FILE}.slo ${SHADER_FILE}.sl
            OUTPUT ${SHADER_FILE}.slo
            DEPENDS ${SHADER_FILE}.sl SeExprOp2)

        add_custom_target(
            shader-${SHADER_BASENAME} ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_BASENAME}.slo)

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_BASENAME}.slo
                DESTINATION prman)
    endforeach()

    file(GLOB ribs "*.rib")
    install(FILES ${ribs} DESTINATION prman)
endif()
