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

## find our parser generators
find_program(BISON_EXE bison)
find_program(FLEX_EXE flex)

# TODO use recent cmake to use FindFlex FindBison
if ((BISON_EXE STREQUAL "BISON_EXE-NOTFOUND") OR
    (FLEX_EXE STREQUAL "FLEX_EXE-NOTFOUND") OR
    (SED_EXE STREQUAL "SED_EXE-NOTFOUND"))
    # don't have flex/bison/sed, use pregenerated versions
    set(parser_cpp generated/ExprParser.cpp generated/ExprLexer.cpp)
else()
    ## build the parser from the flex/yacc sources
    add_custom_command(
        SOURCE "ExprLexer.l"
        COMMAND "flex"
        ARGS "-oExprLexer.cpp" "-PSeExpr2" "${CMAKE_CURRENT_SOURCE_DIR}/ExprLexer.l"
        OUTPUT ExprLexer.cpp
        DEPENDS ExprLexer.l)
    
    add_custom_command(
        SOURCE "ExprParser.y"
        COMMAND "bison"
        ARGS "--defines" "--verbose" "--fixed-output-files" "-p" "SeExpr2" "-b" "ExprParser" "${CMAKE_CURRENT_SOURCE_DIR}/ExprParser.y"
        OUTPUT ExprParser.tab.c ExprParser.tab.h
        DEPENDS ExprParser.y)

    ## needs to be cpp so compiler will find stdlib includes, eg, <vector>
    add_custom_command(
        SOURCE "ExprParser.tab.c"
        COMMAND cp ExprParser.tab.c ExprParser.cpp
        OUTPUT ExprParser.cpp
        DEPENDS ExprParser.tab.c)

    ## set build files
    set(parser_cpp ExprParser.cpp ExprLexer.cpp)
endif()


## Make the SeExpr library with and without LLVM support
file(GLOB llvm_cpp "*.cpp")
add_library(SeExpr2 SHARED ${io_cpp} ${core_cpp} ${parser_cpp} ${llvm_cpp})
if (NOT WIN32)
    target_link_libraries(SeExpr2 "dl" "pthread")
endif()

## Install binary and includes
install(TARGETS SeExpr2 DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include/SeExpr2
        FILES_MATCHING PATTERN "*.h")

# Setup header files
file(GLOB public_includes "*.h")
file(COPY ${public_includes} DESTINATION ${CMAKE_BINARY_DIR}/include/SeExpr2)

configure_file(ExprConfig.h.in ExprConfig.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ExprConfig.h DESTINATION include/SeExpr2)

configure_file(seexpr2.pc.in seexpr2.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/seexpr2.pc DESTINATION share/pkgconfig)

if (ENABLE_LLVM_BACKEND)
    target_link_libraries(SeExpr2 ${LLVM_LIB} "dl" "pthread")
endif()
