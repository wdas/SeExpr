# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

find_package(Doxygen)
IF(DOXYGEN_FOUND AND NOT WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  FILE(GLOB (DOCUMENTED_FILES ../lib/*.h))
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile #  ${DOCUMENTED_FILES}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
  add_dependencies(doc DOCUMENTED_FILES)
  
  ADD_CUSTOM_TARGET(${HTML_TARGET} ALL /usr/bin/doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION share/doc/SeExpr )
  INSTALL( FILES userdoc.txt   DESTINATION share/doc/SeExpr/ RENAME SeExpressions.html )
  INSTALL( FILES Se_voronoi_1.png Se_voronoi_2.png Se_voronoi_3.png Se_voronoi_4.png Se_voronoi_5.png DESTINATION share/doc/SeExpr )
  INSTALL( FILES ui_addWidget.png ui_browser.png ui_editor1.png ui_emptyLayout.png ui_final.png ui_preview.png DESTINATION share/doc/SeExpr )

ENDIF(DOXYGEN_FOUND AND NOT WIN32)
