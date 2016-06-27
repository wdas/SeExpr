
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
