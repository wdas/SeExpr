# SEEXPR SOFTWARE
# Copyright 2011 Disney Enterprises, Inc. All rights reserved
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in
# the documentation and/or other materials provided with the
# distribution.
# 
# * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
# Studios" or the names of its contributors may NOT be used to
# endorse or promote products derived from this software without
# specific prior written permission from Walt Disney Pictures.
# 
# Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
# IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

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
