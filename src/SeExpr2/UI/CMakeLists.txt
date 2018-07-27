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

if (EXISTS "/usr/share/apps/cmake/modules")
    # Needed for some versions of CMake, which only look in version-specific module path
    list(APPEND CMAKE_MODULE_PATH "/usr/share/apps/cmake/modules")
endif()

# Other package dependencies...
find_package(OpenGL)

if (Qt5_FOUND OR QT4_FOUND)

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

    if (ENABLE_QT5)
        qt5_wrap_cpp(EDITOR_MOC_SRCS ${EDITOR_MOC_HDRS})
    else()
        qt4_wrap_cpp(EDITOR_MOC_SRCS ${EDITOR_MOC_HDRS})
    endif()

    set(ANIMLIB_SRCS "")
    if (DEFINED ANIMLIB_DIR)
        set(CE_MOC_HDRS CE/CECurveListUI.h CE/CEDragHandlers.h CE/CEGraphCurve.h
            CE/CEGraphKey.h CE/CEGraphSeg.h CE/CEGraphUI.h CE/CEMainUI.h
            CE/CESegEditUI.h CE/CETool.h)

        set(CE_CPPS CE/CECurveListUI.cpp CE/CEDragHandlers.cpp CE/CEGraphCurve.cpp
            CE/CEGraphKey.cpp CE/CEGraphSeg.cpp CE/CEGraphUI.cpp CE/CEMainUI.cpp
            CE/CESegEditUI.cpp CE/CETool.cpp)

        if (ENABLE_QT5)
            qt5_wrap_cpp(CE_MOC_SRCS ${CE_MOC_HDRS})
        else()
            qt4_wrap_cpp(CE_MOC_SRCS ${CE_MOC_HDRS})
        endif()
        set(ANIMLIB_SRCS "${CE_CPPS} ${CE_MOC_SRCS}")
    endif()

    if (WIN32)
        add_library(SeExpr2Editor ${EDITOR_CPPS} ${EDITOR_MOC_SRCS}
                    ${ANIMLIB_SRCS}
                    ${editor_parser_cpp})
        generate_export_header(SeExpr2Editor
                               BASE_NAME SeExpr2Editor
                               EXPORT_MACRO_NAME SeExpr2Editor
                               EXPORT_FILE_NAME SeExpr2Editor_Export.h
                               STATIC_DEFINE SeExpr2Editor_BUILT_AS_STATIC )
    else()
        add_library(SeExpr2Editor SHARED ${EDITOR_CPPS} ${EDITOR_MOC_SRCS}
                    ${ANIMLIB_SRCS}
                    ${editor_parser_cpp})
    endif()

    set_property(TARGET SeExpr2Editor PROPERTY VERSION ${SeExpr2_VERSION})
    set_property(TARGET SeExpr2Editor PROPERTY SOVERSION 3)
    set_property(TARGET SeExpr2Editor PROPERTY
                 INTERFACE_SeExpr2Editor_MAJOR_VERSION ${SeExpr2_MAJOR_VERSION})
    set_property(TARGET SeExpr2Editor APPEND PROPERTY
                 COMPATIBLE_INTERFACE_STRING ${SeExpr2_MAJOR_VERSION})

    include_directories(${CMAKE_BINARY_DIR}/src/SeExpr2)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(SeExpr2Editor SeExpr2)

    if (ENABLE_QT5)
        target_link_libraries(SeExpr2Editor Qt5::Core)
        target_link_libraries(SeExpr2Editor Qt5::Gui)
        target_link_libraries(SeExpr2Editor Qt5::Widgets)
        target_link_libraries(SeExpr2Editor Qt5::OpenGL)
    else()
        target_link_libraries(SeExpr2Editor ${QT_QTCORE_LIBRARY})
        target_link_libraries(SeExpr2Editor ${QT_QTGUI_LIBRARY})
        target_link_libraries(SeExpr2Editor ${QT_QTOPENGL_LIBRARY})
    endif()
    target_link_libraries(SeExpr2Editor ${OPENGL_LIBRARY})
    target_link_libraries(SeExpr2Editor ${GLUT_LIBRARY})

    if (DEFINED ANIMLIB_DIR)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CE)
        target_link_libraries(SeExpr2Editor animlib)
    endif()

    if (DEFINED DERR_DIR)
        target_link_libraries(SeExpr2Editor DErr)
    endif()

    if (DEFINED DMSG_DIR)
        target_link_libraries(SeExpr2Editor DMsg)
    endif()

    if (DEFINED QDGUI_DIR)
        target_link_libraries(SeExpr2Editor qdcolorwidgets)
    endif()

    if (WIN32)
        target_link_libraries(SeExpr2Editor opengl32)
    endif()

    ## Install library and includes
    install(TARGETS SeExpr2Editor DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${INCLUDE_DIR}/UI
            FILES_MATCHING PATTERN "*.h")

    if(USE_PYTHON)
        # Generate python module expreditor2, using sip
        find_program(SIP_EXEC sip)

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

        if (NOT DEFINED PYQT_SIP_FLAGS)
            if (ENABLE_QT5)
                get_build_info(pyqt5-sip-flags PYQT_SIP_FLAGS)
            else()
                get_build_info(pyqt4-sip-flags PYQT_SIP_FLAGS)
            endif()
            separate_arguments(PYQT_SIP_FLAGS)
        endif()

        if (NOT DEFINED PYQT_SIP_DIR)
            get_build_info(pyqt4-sip PYQT_SIP_DIR)
            if (NOT DEFINED PYQT_SIP_DIR)
               message(FATAL_ERROR "PYQT_SIP_DIR must be defined")
            endif()
        endif()

        set(CMAKE_INSTALL_PYTHON "${PYTHON_SITE}/SeExpr2" )

        set(CMAKE_CXX_FLAGS "-std=c++11")
        include_directories(${SIP_INCLUDE_DIR}
                            ${PYQT_SIP_DIR}
                            ${PYTHON_INCLUDE_DIR})

        add_custom_command(OUTPUT sipexpreditor2part0.cpp
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SeExpr2Editor.sip
                           COMMENT 'Processing SeExpr2Editor.sip'
                           COMMAND ${SIP_EXEC} -w -c .
                           ${PYQT_SIP_FLAGS}
                           -j 1
                           -I. -I${PYQT_SIP_DIR} -I${SIP_INCLUDE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/SeExpr2Editor.sip
                           VERBATIM)

        add_library(expreditor2 SHARED sipexpreditor2part0.cpp)
        target_link_libraries(expreditor2 SeExpr2Editor ${SEEXPR_LIBRARIES}
                              ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${PYTHON_LIBRARIES})

        if (ENABLE_QT5)
            target_link_libraries(expreditor2 SeExpr2Editor
                                  Qt5::Core
                                  Qt5::Gui
                                  Qt5::Widgets
                                  Qt5::OpenGL)
        else()
            target_link_libraries(expreditor2 SeExpr2Editor
                                  ${QT_QTCORE_LIBRARY}
                                  ${QT_QTGUI_LIBRARY}
                                  ${QT_QTOPENGL_LIBRARY})
        endif()

        # No prefix on python module name
        set_target_properties(expreditor2 PROPERTIES PREFIX "")
        install(TARGETS expreditor2 DESTINATION ${CMAKE_INSTALL_PYTHON})
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
                DESTINATION ${CMAKE_INSTALL_PYTHON})
    endif()
endif()
