
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
SET(RMANTREE_TEST $ENV{RMANTREE} CACHE STRING stuff FORCE)
STRING(COMPARE EQUAL "${RMANTREE_TEST}" "" RMAN_SET)

IF(NOT ${RMAN_SET})
include_directories($ENV{RMANTREE}/include)
link_directories($ENV{RMANTREE}/lib)
add_library(SeExprOp SHARED seop.cpp)
target_link_libraries(SeExprOp prman ${SEEXPR_LIBRARIES})
FILE(GLOB ribs "*.rib")
install (TARGETS SeExprOp DESTINATION ${CMAKE_INSTALL_LIBDIR}/prman)

foreach (SHADERSOURCE testdisp testse)
    ADD_CUSTOM_COMMAND(
      SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl
      COMMAND "shader"
      ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl 
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl SeExprOp
    )
    add_custom_target(shader-${SHADERSOURCE} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo DESTINATION ${CMAKE_INSTALL_LIBDIR}/prman)
endforeach(SHADERSOURCE)
install (FILES ${ribs} DESTINATION ${CMAKE_INSTALL_LIBDIR}/prman)
endif(NOT ${RMAN_SET})
