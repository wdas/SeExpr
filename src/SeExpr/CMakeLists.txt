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
set_source_files_properties("SeExprBuiltins.cpp" PROPERTIES COMPILE_DEFINITIONS "__STDC_LIMIT_MACROS")
if(NOT ENABLE_LLVM_BACKEND)
  file(GLOB to_remove "SeExprLLVMCodeGeneration.cpp")
  list(REMOVE_ITEM io_cpp ${to_remove})
endif()

## find our parser generators
find_program(BISON_EXE bison)
find_program(FLEX_EXE flex)
find_program(SED_EXE sed)

# TODO use recent cmake to use FindFlex FindBison
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
#      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
      ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'"  SeExprParserLexIn.cpp | tee SeExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParserLex.cpp > /dev/null
      OUTPUT SeExprParserLex.cpp
      DEPENDS SeExprParserLexIn.cpp
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "SeExprParser.y"
      COMMAND "bison"
      ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "SeExpr2" "${CMAKE_CURRENT_SOURCE_DIR}/SeExprParser.y"
      OUTPUT y.tab.c y.tab.h
      DEPENDS SeExprParser.y
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.h"
      COMMAND "sed"
      ARGS -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'" y.tab.h | tee  SeExprParser.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.tab.h > /dev/null
      OUTPUT SeExprParser.tab.h
      DEPENDS y.tab.h
    )
    
    ADD_CUSTOM_COMMAND(
      SOURCE "y.tab.c"
      COMMAND "sed"
      ARGS -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'" y.tab.c | tee SeExprParser.cpp  "${CMAKE_CURRENT_SOURCE_DIR}/generated/SeExprParser.cpp" > /dev/null
      OUTPUT SeExprParser.cpp
      DEPENDS y.tab.c SeExprParser.tab.h
    )

    ## set build files
    set (parser_cpp SeExprParser.cpp SeExprParserLex.cpp )

endif( (BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND")  OR (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))

## Make the SeExpr library
ADD_LIBRARY (SeExpr2 SHARED ${io_cpp} ${core_cpp} ${parser_cpp})
IF(NOT WIN32)
   set(SEEXPR_LINK_LIB "dl" "pthread")
   IF(ENABLE_LLVM_BACKEND)
      list(APPEND SEEXPR_LINK_LIB ${REQ_LLVM_LIBRARIES})
   ENDIF()
   TARGET_LINK_LIBRARIES(SeExpr2 ${SEEXPR_LINK_LIB})
ENDIF(NOT WIN32)
## Install binary and includes
FILE(GLOB public_includes "*.h")
INSTALL (TARGETS SeExpr2 DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL (FILES ${public_includes} DESTINATION include)

if (ENABLE_LLVM_BACKEND)
## remove conditional inlucde in header file
ADD_CUSTOM_COMMAND(
   OUTPUT SeExpression.h
   COMMAND unifdef -DSEEXPR_ENABLE_LLVM -o SeExpression.h ${CMAKE_CURRENT_SOURCE_DIR}/SeExpression.h || true #unifdef not always return 0 when succeed. 
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SeExpression.h
)
add_custom_target(SeExpression_Header ALL DEPENDS SeExpression.h)
INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/SeExpression.h DESTINATION include)
endif()
