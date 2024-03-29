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

# Source files for llvm supported library and interpreter library
file(GLOB io_cpp "*.cpp")
file(GLOB to_remove "ExprLLVMCodeGeneration.cpp")
list(REMOVE_ITEM io_cpp ${to_remove})

set_source_files_properties("ExprBuiltins.cpp" PROPERTIES COMPILE_DEFINITIONS "__STDC_LIMIT_MACROS")

# Uncomment below to print debug messages / performance stats
#add_definitions(-DSEEXPR_DEBUG)
#add_definitions(-DSEEXPR_PERFORMANCE)

# Allow flex/bison to find the current directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

## find our parser generators
find_program(BISON_EXE bison)
find_program(FLEX_EXE flex)
find_program(SED_EXE sed)

# TODO use recent cmake to use FindFlex FindBison
if ((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR
    (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND") OR
    (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
  # don't have flex/bison/sed, use pregenerated versions
    set(parser_cpp ${PREGENERATED_ROOT}/SeExpr/generated/ExprParser.cpp ${PREGENERATED_ROOT}/SeExpr/generated/ExprParserLex.cpp)
else()
    ## build the parser from the flex/yacc sources
    add_custom_command(
        SOURCE "ExprParserLex.l"
        COMMAND "flex"
        ARGS "-oExprParserLexIn.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ExprParserLex.l"
        OUTPUT ExprParserLexIn.cpp
        DEPENDS ExprParserLex.l)
    
    add_custom_command(
        SOURCE "ExprParserLexIn.cpp"
        COMMAND "sed"
        ARGS -e "'s/SeExprwrap(n)/SeExprwrap()/g'" -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'"  ExprParserLexIn.cpp | tee ExprParserLex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/generated/ExprParserLex.cpp > /dev/null
        OUTPUT ExprParserLex.cpp
        DEPENDS ExprParserLexIn.cpp)
    
    add_custom_command(
        SOURCE "ExprParser.y"
        COMMAND "bison"
        ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "SeExpr2" "${CMAKE_CURRENT_SOURCE_DIR}/ExprParser.y"
        OUTPUT y.tab.c y.tab.h
        DEPENDS ExprParser.y)
    
    add_custom_command(
        SOURCE "y.tab.h"
        COMMAND "sed"
        ARGS -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'" y.tab.h | tee  ExprParser.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/generated/ExprParser.tab.h > /dev/null
        OUTPUT ExprParser.tab.h
        DEPENDS y.tab.h)
    
    add_custom_command(
        SOURCE "y.tab.c"
        COMMAND "sed"
        ARGS -e "'s/yy/SeExpr2/g'" -e "'s/YY/SeExprYY/g'" y.tab.c | tee ExprParser.cpp  "${CMAKE_CURRENT_SOURCE_DIR}/generated/ExprParser.cpp" > /dev/null
        OUTPUT ExprParser.cpp
        DEPENDS y.tab.c ExprParser.tab.h)

    ## set build files
    set(parser_cpp ExprParser.cpp ExprParserLex.cpp)
endif()


## Make the SeExpr library with and without LLVM support
file(GLOB llvm_cpp "*.cpp")
if (NOT WIN32)
    add_library(SeExpr2 SHARED ${io_cpp} ${core_cpp} ${parser_cpp} ${llvm_cpp})
    target_link_libraries(SeExpr2 "dl" "pthread")
else()
    add_library(SeExpr2 STATIC ${io_cpp} ${core_cpp} ${parser_cpp} ${llvm_cpp})
endif()

set_property(TARGET SeExpr2 PROPERTY VERSION ${seexpr2_VERSION})
set_property(TARGET SeExpr2 PROPERTY SOVERSION ${seexpr2_MAJOR_VERSION})
set_property(TARGET SeExpr2 PROPERTY
             INTERFACE_SeExpr2_MAJOR_VERSION ${seexpr2_MAJOR_VERSION})
set_property(TARGET SeExpr2 APPEND PROPERTY
             COMPATIBLE_INTERFACE_STRING ${seexpr2_MAJOR_VERSION})

generate_export_header(SeExpr2)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/seexpr2_export.h"
        COMPONENT devel DESTINATION ${INCLUDE_DIR})

# When targets are consumed downstream they should only add the top-level
# $prefix/include directory.  Downstream consumers are expected to
# use #include <project-name/header.h>.
target_include_directories(SeExpr2 PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

## Install binary and includes
install(TARGETS SeExpr2
        EXPORT ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
        COMPONENT devel DESTINATION ${INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")

configure_file("ExprConfig.h.in" "ExprConfig.h")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ExprConfig.h"
        COMPONENT devel DESTINATION ${INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (ENABLE_LLVM_BACKEND)
    if (NOT WIN32)
        target_link_libraries(SeExpr2 ${LLVM_LIB} "dl" "pthread")
    else ()
        target_link_libraries(SeExpr2 ${LLVM_LIB})
    endif ()
endif()
