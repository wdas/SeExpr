
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

find_package(OpenGL)
find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL) # find and setup Qt4 for this project
if(QT4_FOUND)

    BuildParserScanner(SeExprSpecParserLex SeExprSpecParser SeExprSpec editor_parser_cpp)
    set(SeExprEd_MOC_HDRS SeExprEdBrowser.h SeExprEdColorCurve.h SeExprEdControlCollection.h
      SeExprEdControl.h SeExprEdCurve.h SeExprEdDialog.h SeExprEditor.h SeExprEdFileDialog.h SeExprEdGrapher2d.h
      SeExprEdPopupDocumentation.h SeExprEdShortEdit.h)
    set(SeExprEd_CPPS SeExprEdFileDialog.cpp SeExprEdControl.cpp SeExprEditor.cpp SeExpressionEditor.cpp
      SeExprEdShortEdit.cpp SeExprEdCurve.cpp  SeExprEdColorCurve.cpp SeExprEdEditableExpression.cpp 
      SeExprEdPopupDocumentation.cpp SeExprEdCompletionModel.cpp SeExprEdDialog.cpp SeExprEdControlCollection.cpp
      SeExprEdGrapher2d.cpp SeExprEdBrowser.cpp SeExprEdExpression.cpp)

    qt4_wrap_cpp(SeExprEd_MOC_SRCS ${SeExprEd_MOC_HDRS})
    
    add_library(SeExprEditor SHARED ${SeExprEd_CPPS} ${SeExprEd_MOC_SRCS} ${editor_parser_cpp})
IF(WIN32)
    add_library(SeExprEditor ${SeExprEd_CPPS} ${SeExprEd_MOC_SRCS} ${editor_parser_cpp})
    GENERATE_EXPORT_HEADER( SeExprEditor
             BASE_NAME SeExprEditor
             EXPORT_MACRO_NAME SeExprEditor
             EXPORT_FILE_NAME SeExprEditor_Export.h
             STATIC_DEFINE SeExprEditor_BUILT_AS_STATIC
    )
ENDIF(WIN32)

    
    install(TARGETS SeExprEditor DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SeExprEd_MOC_HDRS} DESTINATION include/SeExprEditor)
    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(SeExprEditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTGUI_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExprEditor ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

IF(WIN32)
    target_link_libraries(SeExprEditor opengl32)
ENDIF(WIN32)
    target_link_libraries(SeExprEditor ${SEEXPR_LIBRARIES})
endif(QT4_FOUND)
