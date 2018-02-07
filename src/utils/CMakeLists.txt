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

include_directories(${CMAKE_BINARY_DIR}/src/SeExpr2)

foreach(item eval listVar)
    add_executable("${item}" "${item}.cpp")
    target_link_libraries("${item}" ${SEEXPR_LIBRARIES})
    install(TARGETS "${item}" DESTINATION share/SeExpr2/utils)
endforeach()
