
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

FILE(GLOB io_cpp "*.cpp")


## find our parser generators
#find_program(BISON_EXE bison)
#find_program(FLEX_EXE flex)
#find_program(SED_EXE sed)
#
#if((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
#     # don't have flex/bison/sed, use pregenerated versions
#    set (parser_cpp generated/SeExprParser.cpp generated/SeExprParserLex.cpp )
#else ((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
#     ## build the parser from the flex/yacc sources
#
#    ADD_CUSTOM_COMMAND(
#      SOURCE "SeExprParserLex.l"
#      COMMAND "flex"
#      ARGS "-oSeExprParserLexIn.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/SeExprParserLex.l"
#      OUTPUT SeExprParserLexIn.cpp
#      DEPENDS SeExprParserLex.l
#    )
#    
#    ADD_CUSTOM_COMMAND(
#      SOURCE "SeExprParserLexIn.cpp"
#      COMMAND "sed"
##      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
#      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
#      OUTPUT SeExprParserLex.cpp
#      DEPENDS SeExprParserLexIn.cpp
#    )
#    
#    ADD_CUSTOM_COMMAND(
#      SOURCE "SeExprParser.y"
#      COMMAND "bison"
#      ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "SeExpr" "${CMAKE_CURRENT_SOURCE_DIR}/SeExprParser.y"
#      OUTPUT y.tab.c y.tab.h
#      DEPENDS SeExprParser.y
#    )
#    
#    ADD_CUSTOM_COMMAND(
#      SOURCE "y.tab.h"
#      COMMAND "sed"
#      ARGS -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'" y.tab.h | tee  SeExprParser.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.tab.h > /dev/null
#      OUTPUT SeExprParser.tab.h
#      DEPENDS y.tab.h
#    )
#    
#    ADD_CUSTOM_COMMAND(
#      SOURCE "y.tab.c"
#      COMMAND "sed"
#      ARGS -e "'s/yy/SeExpr/g'" -e "'s/YY/SeExprYY/g'" y.tab.c | tee SeExprParser.cpp  "${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.cpp" > /dev/null
#      OUTPUT SeExprParser.cpp
#      DEPENDS y.tab.c SeExprParser.tab.h
#    )
#
#    ## set build files
#    set (parser_cpp SeExprParser.cpp SeExprParserLex.cpp )
#
#endif( (BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))

BuildParserScanner(SeExprParserLex SeExprParser SeExpr parser_cpp)


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
INSTALL (TARGETS SeExpr SeExpr-static DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL (FILES ${public_includes} DESTINATION include)
