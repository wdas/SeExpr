
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
IF(DOXYGEN_FOUND AND NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  FILE(GLOB (DOCUMENTED_FILES ../lib/*.h))

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.txt
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile >doxygen.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)

  add_custom_target(doc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.txt)

  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION share/doc/SeExpr )
  INSTALL( FILES userdoc.txt   DESTINATION share/doc/SeExpr/ RENAME SeExpressions.html )
  INSTALL( FILES Se_voronoi_1.png Se_voronoi_2.png Se_voronoi_3.png Se_voronoi_4.png Se_voronoi_5.png DESTINATION share/doc/SeExpr )
  INSTALL( FILES ui_addWidget.png ui_browser.png ui_editor1.png ui_emptyLayout.png ui_final.png ui_preview.png DESTINATION share/doc/SeExpr )

ENDIF(DOXYGEN_FOUND AND NOT WIN32)
