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

FILE(GLOB io_cpp "*.cpp")

## find our parser generators
find_program(BISON_EXE bison)
find_program(FLEX_EXE flex)
find_program(SED_EXE sed)

if((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
     # don't have flex/bison/sed, use pregenerated versions
    set (parser_cpp generated/SeExprParser.cpp generated/SeExprParserLex.cpp )
else ((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
     ## build the parser from the flex/yacc sources

    ADD_CUSTOM_COMMAND(
      SOURCE "SeExprParserLex.l"
      COMMAND "flex"
      ARGS "-oSeExprParserLexIn.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/SeExprParserLex.l"
      OUTPUT SeExprParserLexIn.cpp
      DEPENDS SeExprParserLex.l
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "SeExprParserLexIn.cpp"
      COMMAND "sed"
#      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
      OUTPUT SeExprParserLex.cpp
      DEPENDS SeExprParserLexIn.cpp
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "SeExprParser.y"
      COMMAND "bison"
      ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "SeExpr" "${CMAKE_CURRENT_SOURCE_DIR}/SeExprParser.y"
      OUTPUT y.tab.c y.tab.h
      DEPENDS SeExprParser.y
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.h"
      COMMAND "sed"
      ARGS -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'" y.tab.h | tee  SeExprParser.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.tab.h > /dev/null
      OUTPUT SeExprParser.tab.h
      DEPENDS y.tab.h
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.c"
      COMMAND "sed"
      ARGS -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'" y.tab.c | tee SeExprParser.cpp  "${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.cpp" > /dev/null
      OUTPUT SeExprParser.cpp
      DEPENDS y.tab.c SeExprParser.tab.h
    )

    ## set build files
    set (parser_cpp SeExprParser.cpp SeExprParserLex.cpp )

endif( (BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))

## Make the SeExpr library
ADD_LIBRARY (SeExpr SHARED ${io_cpp} ${core_cpp} ${parser_cpp})
ADD_LIBRARY (SeExpr-static  ${io_cpp} ${core_cpp} ${parser_cpp})
SET_TARGET_PROPERTIES(SeExpr-static PROPERTIES OUTPUT_NAME "SeExpr")


IF(NOT WIN32)
   TARGET_LINK_LIBRARIES(SeExpr "dl" "pthread")
   TARGET_LINK_LIBRARIES(SeExpr-static "dl" "pthread")
ENDIF(NOT WIN32)
## Install binary and includes
FILE(GLOB public_includes "*.h")
IF (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR "lib64")
ENDIF (NOT DEFINED CMAKE_INSTALL_LIBDIR)
INSTALL (TARGETS SeExpr SeExpr-static DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL (FILES ${public_includes} DESTINATION include)
