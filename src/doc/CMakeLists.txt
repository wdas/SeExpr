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

find_package(Doxygen)
if (DOXYGEN_FOUND AND NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  file(GLOB (DOCUMENTED_FILES ../lib/*.h))

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.txt
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile >doxygen.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)

  add_custom_target(doc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.txt)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION share/doc/SeExpr2)
  install(FILES userdoc.txt DESTINATION share/doc/SeExpr2/ RENAME SeExpressions.html)

  file(GLOB "*.png" PNG_FILES)
  install(FILES ${PNG_FILES} DESTINATION share/doc/SeExpr2)

endif()
