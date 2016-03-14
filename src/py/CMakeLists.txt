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


EXECUTE_PROCESS( COMMAND python -c "import sys;print\"%s.%s\"%sys.version_info[0:2]" OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS( COMMAND  which python OUTPUT_VARIABLE PYTHON_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS( COMMAND  bash -c "ldd `which python` | grep libpyth | awk '{print $3}'" OUTPUT_VARIABLE PYTHON_LIBRARY OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS( COMMAND python-config --includes OUTPUT_VARIABLE PYTHON_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
#FIND_PACKAGE(PythonLibs)
string(REGEX REPLACE "^[-I]" "" PYTHON_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
string(REGEX REPLACE "[ ]-I" " " PYTHON_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
separate_arguments(PYTHON_INCLUDE_DIR)

message(${PYTHON_INCLUDE_DIR})
#find_package_handle_standard_args(PythonDev DEFAULT_MSG
#    PYTHON_CONFIG
#    PYTHON_INCLUDE_DIR
#    PYTHON_LIBRARIES
#)
#FIND_PACKAGE(PythonLibs)
#IF(PYTHONLIBS_FOUND)
SET(PYTHON_DEST "lib64/python${PYTHON_VERSION}/site-packages" CACHED)

include_directories($ENV{RP_boost_disney}/include/boost-disney/boost)
include_directories(${PYTHON_INCLUDE_DIR})
link_directories($ENV{RP_boost_disney}/lib64)
include_directories(../SeExpr/parser)
add_library(core SHARED SeExprPy.cpp ../SeExpr/parser/SeExprParse.cpp ../SeExpr/parser/SeExprLex.cpp)
target_link_libraries(core ${PYTHON_LIBRARY})
target_link_libraries(core boost_python-mt)
#target_link_libraries(core python${PYTHON_VERSION})
set_target_properties(core PROPERTIES PREFIX "")
SET(PYTHON_DEST "lib64/python${PYTHON_VERSION}/site-packages/SeExprPy" )
#INSTALL (FESTINATION include)
INSTALL (TARGETS core DESTINATION ${PYTHON_DEST})
INSTALL (FILES utils.py DESTINATION ${PYTHON_DEST})
INSTALL (FILES __init__.py DESTINATION ${PYTHON_DEST})
