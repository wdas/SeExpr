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

# Find python includes and libs. cmake doesn't seem to handle automatically

set(PFMAKEVAR "pf-makevar")

execute_process(COMMAND ${PFMAKEVAR} python-ver
        OUTPUT_VARIABLE PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${PFMAKEVAR} python-inc
        OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${PFMAKEVAR} python-lib
        OUTPUT_VARIABLE PYTHON_LIB_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${PFMAKEVAR} python-site
        OUTPUT_VARIABLE PYTHON_SITE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

FIND_LIBRARY(PYTHON_LIBRARY NAMES "python${PYTHON_VERSION}"
        PATHS "${PYTHON_LIB_DIR}" NO_DEFAULT_PATH)

find_package(PythonInterp)
find_package(PythonLibs)

# Other package dependencies...

find_package(OpenGL)
find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL)

set(PYQT4_PATH /usr/share/sip/PyQt4)               # for qt sip files
set(SIP_PATH /usr/include/python${PYTHON_VERSION}) # for sip.h

if(QT4_FOUND)

    BuildParserScanner(SeExprSpecParserLex SeExprSpecParser SeExprEdSpec
                       editor_parser_cpp)

    set(SeExprEd_MOC_HDRS SeExprEdBrowser.h SeExprEdColorCurve.h
        SeExprEdColorSwatchWidget.h SeExprEdControlCollection.h
        SeExprEdControl.h SeExprEdCurve.h SeExprEdDialog.h SeExprEditor.h
        SeExprEdFileDialog.h SeExprEdGrapher2d.h SeExprEdPopupDocumentation.h
        SeExprEdShortEdit.h)

    set(SeExprEd_CPPS SeExprEdFileDialog.cpp SeExprEdControl.cpp
        SeExprEditor.cpp SeExpressionEditor.cpp SeExprEdShortEdit.cpp
        SeExprEdCurve.cpp  SeExprEdColorCurve.cpp SeExprEdColorSwatchWidget.cpp
        SeExprEdEditableExpression.cpp SeExprEdPopupDocumentation.cpp
        SeExprEdCompletionModel.cpp SeExprEdDialog.cpp
        SeExprEdControlCollection.cpp SeExprEdGrapher2d.cpp SeExprEdBrowser.cpp
        SeExprEdExpression.cpp )

    qt4_wrap_cpp(SeExprEd_MOC_SRCS ${SeExprEd_MOC_HDRS})

    add_library(SeExprEditor SHARED ${SeExprEd_CPPS} ${SeExprEd_MOC_SRCS}
                ${editor_parser_cpp})

IF(WIN32)
    add_library(SeExprEditor ${SeExprEd_CPPS} ${SeExprEd_MOC_SRCS}
                ${editor_parser_cpp})
    GENERATE_EXPORT_HEADER(SeExprEditor
                           BASE_NAME SeExprEditor
                           EXPORT_MACRO_NAME SeExprEditor
                           EXPORT_FILE_NAME SeExprEditor_Export.h
                           STATIC_DEFINE SeExprEditor_BUILT_AS_STATIC )
ENDIF(WIN32)

    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(SeExprEditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTGUI_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExprEditor ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

IF(WIN32)
    target_link_libraries(SeExprEditor opengl32)
ENDIF(WIN32)

    target_link_libraries(SeExprEditor ${SEEXPR_LIBRARIES})

    install(TARGETS SeExprEditor DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SeExprEd_MOC_HDRS}
            DESTINATION include/SeExprEditor)

    # Generate python module expreditor, using sip

    find_program(SIP_EXEC sip)
    set(SIP_QT_TAG -t Qt_4_5_0)  # fix 'sip: QFileIconProvider is undefined'

    set(CMAKE_CXX_FLAGS "-std=c++0x")
    set(CMAKE_INSTALL_PYTHON "${CMAKE_INSTALL_PREFIX}/${PYTHON_SITE}/SeExpr/" )
    include_directories(${SIP_PATH} ${PYQT4_PATH}
                        ${PYTHON_INCLUDE_DIR} ${QT_INCLUDE_DIR}/QtCore
                        ${QT_INCLUDE_DIR}/QtGui)

    add_custom_command(OUTPUT sipexpreditorpart0.cpp
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SeExprEditor.sip
                       COMMENT 'Processing SeExprEditor.sip'
                       COMMAND ${SIP_EXEC} -w -c .
                       -x VendorID -x PyQt_NoPrintRangeBug -x Py_v3
                       -t WS_X11 -g -j 1
                       ${SIP_QT_TAG}
                       -I. -I${PYQT4_PATH} -I${SIP_PATH}
                       ${CMAKE_CURRENT_SOURCE_DIR}/SeExprEditor.sip
                       VERBATIM)

    add_library(expreditor SHARED sipexpreditorpart0.cpp)
    target_link_libraries(expreditor SeExprEditor ${SEEXPR_LIBRARIES}
                          ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                          ${QT_QTOPENGL_LIBRARY} ${OPENGL_LIBRARY}
                          ${GLUT_LIBRARY} ${PYTHON_LIBRARIES})

    # No prefix on python module name
    set_target_properties(expreditor PROPERTIES PREFIX "")
    install(TARGETS expreditor DESTINATION ${CMAKE_INSTALL_PYTHON})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
            DESTINATION ${CMAKE_INSTALL_PYTHON})

endif(QT4_FOUND)
