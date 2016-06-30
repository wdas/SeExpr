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

find_package(PythonInterp)
find_package(PythonLibs)

macro(get_build_info NAME STORAGE)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/build/build-info ${NAME}
        OUTPUT_VARIABLE ${STORAGE}
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endmacro()

get_build_info(python-site PYTHON_SITE)
get_build_info(python-inc PYTHON_INCLUDE_DIR)
get_build_info(sip-inc SIP_INCLUDE_DIR)
get_build_info(pyqt4-sip PYQT4_SIP)

set(CMAKE_INSTALL_PYTHON "${PYTHON_SITE}/SeExpr2" )

# Other package dependencies...

find_package(OpenGL)
find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL)

if(QT4_FOUND)

    BuildParserScanner(ExprSpecParserLex ExprSpecParser ExprSpec
                       editor_parser_cpp)

    set(EDITOR_MOC_HDRS ExprBrowser.h ExprColorCurve.h
        ExprColorSwatch.h ExprControlCollection.h
        ExprControl.h ExprCurve.h ExprDialog.h ExprEditor.h
        ExprFileDialog.h ExprGrapher2d.h ExprPopupDoc.h
        ExprShortEdit.h ExprDeepWater.h)

    set(EDITOR_CPPS ExprFileDialog.cpp ExprControl.cpp
        ExprEditor.cpp ExprMain.cpp ExprShortEdit.cpp
        ExprCurve.cpp  ExprColorCurve.cpp ExprColorSwatch.cpp
        EditableExpression.cpp ExprPopupDoc.cpp
        ExprCompletionModel.cpp ExprDialog.cpp
        ExprControlCollection.cpp ExprGrapher2d.cpp ExprBrowser.cpp
        BasicExpression.cpp ExprDeepWater.cpp)

    qt4_wrap_cpp(EDITOR_MOC_SRCS ${EDITOR_MOC_HDRS})

    add_library(SeExpr2Editor SHARED ${EDITOR_CPPS} ${EDITOR_MOC_SRCS}
                ${editor_parser_cpp})

IF(WIN32)
    add_library(SeExpr2Editor ${EDITOR_CPPS} ${EDITOR_MOC_SRCS}
                ${editor_parser_cpp})
    GENERATE_EXPORT_HEADER(SeExpr2Editor
                           BASE_NAME SeExpr2Editor
                           EXPORT_MACRO_NAME SeExpr2Editor
                           EXPORT_FILE_NAME SeExpr2Editor_Export.h
                           STATIC_DEFINE SeExpr2Editor_BUILT_AS_STATIC )
ENDIF(WIN32)

    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/CE)
    target_link_libraries(SeExpr2Editor ${QT_QTCORE_LIBRARY})
    target_link_libraries(SeExpr2Editor ${QT_QTGUI_LIBRARY})
    target_link_libraries(SeExpr2Editor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExpr2Editor ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

IF(WIN32)
    target_link_libraries(SeExpr2Editor opengl32)
ENDIF(WIN32)

    install(TARGETS SeExpr2Editor DESTINATION ${CMAKE_INSTALL_LIBDIR})

    # Setup header files
    FILE(GLOB public_includes "*.h")
    FILE(COPY ${public_includes} DESTINATION ${CMAKE_BINARY_DIR}/include/SeExpr2/UI)

    # Generate python module expreditor2, using sip
    find_program(SIP_EXEC sip)
    set(SIP_QT_TAG -t Qt_4_8_0)

    set(CMAKE_CXX_FLAGS "-std=c++0x")
    include_directories(${SIP_INCLUDE_DIR} ${PYQT4_SIP}
                        ${PYTHON_INCLUDE_DIR} ${QT_INCLUDE_DIR}/QtCore
                        ${QT_INCLUDE_DIR}/QtGui)

    add_custom_command(OUTPUT sipexpreditor2part0.cpp
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SeExpr2Editor.sip
                       COMMENT 'Processing SeExpr2Editor.sip'
                       COMMAND ${SIP_EXEC} -w -c .
                       -x VendorID -x PyQt_NoPrintRangeBug -x Py_v3
                       ${SIP_QT_TAG}
                       -t WS_X11 -g -j 1
                       -I. -I${PYQT4_SIP} -I${SIP_INCLUDE_DIR}
                       ${CMAKE_CURRENT_SOURCE_DIR}/SeExpr2Editor.sip
                       VERBATIM)

    add_library(expreditor2 SHARED sipexpreditor2part0.cpp)
    target_link_libraries(expreditor2 SeExpr2Editor ${SEEXPR_LIBRARIES}
                          ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                          ${QT_QTOPENGL_LIBRARY} ${OPENGL_LIBRARY}
                          ${GLUT_LIBRARY} ${PYTHON_LIBRARIES})

    # No prefix on python module name
    set_target_properties(expreditor2 PROPERTIES PREFIX "")
    install(TARGETS expreditor2 DESTINATION ${CMAKE_INSTALL_PYTHON})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
            DESTINATION ${CMAKE_INSTALL_PYTHON})

endif(QT4_FOUND)
