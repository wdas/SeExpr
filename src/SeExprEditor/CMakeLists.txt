# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

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
    
    install(TARGETS SeExprEditor DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SeExprEd_MOC_HDRS} DESTINATION include/SeExprEditor)
    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(SeExprEditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTGUI_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExprEditor ${SEEXPR_LIBRARIES})
endif(QT4_FOUND)
