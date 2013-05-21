# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

# Non optimal, can't figure out better way right now
SET(RMANTREE_TEST $ENV{RMANTREE} CACHE STRING stuff FORCE)
STRING(COMPARE EQUAL "${RMANTREE_TEST}" "" RMAN_SET)

IF(NOT ${RMAN_SET})
include_directories($ENV{RMANTREE}/include)
link_directories($ENV{RMANTREE}/lib)
add_library(SeExprOp SHARED seop.cpp)
target_link_libraries(SeExprOp prman ${SEEXPR_LIBRARIES})
FILE(GLOB ribs "*.rib")
install (TARGETS SeExprOp DESTINATION prman)

foreach (SHADERSOURCE testdisp testse)
    ADD_CUSTOM_COMMAND(
      SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl
      COMMAND "shader"
      ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl 
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADERSOURCE}.sl SeExprOp
    )
    add_custom_target(shader-${SHADERSOURCE} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SHADERSOURCE}.slo DESTINATION prman)
endforeach(SHADERSOURCE)
install (FILES ${ribs} DESTINATION prman)
endif(NOT ${RMAN_SET})
